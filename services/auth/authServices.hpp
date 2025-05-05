
#pragma once
#include <jwt-cpp/jwt.h>
#include <stdio.h>

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <tuple>

#include "auth/auth.h"
#include "bcrypt/BCrypt.hpp"
#include "dbclient.h"
#include "jwt-cpp/traits/nlohmann-json/traits.h"
using json = nlohmann::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using traits = jwt::traits::nlohmann_json;
using claim = jwt::basic_claim<traits>;
using sec = std::chrono::seconds;
using min = std::chrono::minutes;

namespace services {
namespace auth {
enum class CollectionType { Account, User };

std::optional<std::string> validate_email(const std::string& email) {
  const std::regex pattern(
      R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
  if (std::regex_match(email, pattern)) {
    return std::nullopt;
  }
  return "Email must be formatted as user@example.com";
}

std::optional<std::string> validate_password(const std::string& password) {
  const std::regex pattern(
      R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,}$)");
  if (std::regex_match(password, pattern)) {
    return std::nullopt;
  }
  return "Password must contain at least one lowercase letter, one uppercase "
         "letter, one digit, and one special character.";
}

std::optional<std::string> validate_username(const std::string& username) {
  const std::regex pattern(R"(^(?!.*[._]{2})[a-zA-Z0-9._]+$)");
  if (std::regex_match(username, pattern)) {
    return std::nullopt;
  }
  return "Username must not have consecutive dots or underscores and must "
         "only contain letters, digits, dots, or underscores.";
}

std::tuple<bool, std::optional<std::string>> validate_account(
    const models::auth::SignUpRequestBody& acc) {
  auto email_error = validate_email(acc.email);
  if (email_error) {
    return {false, email_error};
  }

  auto password_error = validate_password(acc.password);
  if (password_error) {
    return {false, password_error};
  }

  auto username_error = validate_username(acc.user_name);
  if (username_error) {
    return {false, username_error};
  }

  return {true, std::nullopt};
}

// todo: make services have DBConnected concept and make it class based
mongocxx::collection get_collection(CollectionType c) {
  mongocxx::pool::entry client = utils::MongoPool::acquire();
  // I think there's a better way to handle this, maybe passing in the client
  // as opposed to doing it here?
  auto db = client->database(utils::DB_NAME);

  if (c == CollectionType::Account) return db["user_account"];

  return db["user_profile"];
}

const std::string generate_password_hash(const std::string& password) {
  return BCrypt::generateHash(password);
}

//   const bool handle_(const std::string& password,
//                                const std::string& hashed_password) {
//     return BCrypt::validatePassword(password, hashed_password);
//   }

// at this point assume that everything is valid
std::tuple<json, std::optional<json>> handle_sign_up(
    const models::auth::SignUpRequestBody body) {
  const std::string password_hash{generate_password_hash(body.password)};

  const models::auth::SignUpRequestBody user_account{
      .user_name = body.user_name,
      .password = password_hash,
      .email = body.email};

  std::optional<mongocxx::v_noabi::result::insert_one> result;

  // maybe have error middleware?
  // or maybe just pass the result object here idk
  try {
    auto account_collection = get_collection(CollectionType::Account);
    auto profile_collection = get_collection(CollectionType::User);

    auto existing_record = account_collection.find_one(
        make_document(kvp("user_name", body.user_name)));

    if (!existing_record) {
      json values =
          models::serialize<models::auth::SignUpRequestBody>(user_account);

      result = account_collection.insert_one(bsoncxx::from_json(values.dump()));

      profile_collection.insert_one(
          make_document(kvp("_id", result->inserted_id().get_oid().value)));
    } else {
      std::cout << "NAH BRO HE EXISTS" << std::endl;
      return std::make_tuple(
          json{}, json{{"error", "User already exists"}, {"code", 400}});
    }
  } catch (const std::exception& e) {
    std::cout << "ERROR WHEN ADDING A USER: " << e.what() << std::endl;
    return std::make_tuple(
        json{}, json{{"error", "Internal server error"}, {"code", 500}});
  }

  if (result && result->result().inserted_count() == 1) {
    const auto time = jwt::date::clock::now();
    auto access_token =
        jwt::create<traits>()
            .set_type("JWT")
            .set_issuer("auth.dummy_project")
            .set_audience("dummy_project")
            .set_issued_at(time)
            .set_not_before(time)
            .set_expires_at(time + sec{3600})
            .set_payload_claim(
                "id", claim(result->inserted_id().get_oid().value.to_string()))
            .sign(jwt::algorithm::none{});

    // TODO: store JTI claim in db and properly genenrate it with uuid

    auto refresh_token =
        jwt::create<traits>()
            .set_type("JWT")
            .set_issuer("auth.dummy_project")
            .set_audience("dummy_project")
            .set_issued_at(time)
            .set_not_before(time)
            .set_expires_at(time + min{10080})
            .set_payload_claim(
                "id", claim(result->inserted_id().get_oid().value.to_string()))
            .set_payload_claim("jti", claim(std::string("fhqweruifqowefhoqwe")))
            .sign(jwt::algorithm::none{});

    bsoncxx::oid inserted_id = result->inserted_id().get_oid().value;
    return {json{{"userId", inserted_id.to_string()},
                 {"access_token", access_token},
                 {"refresh_token", refresh_token}},
            std::nullopt};
  }

  return std::make_tuple(json{},
                         json{{"error", "Insert failed"}, {"code", 500}});
}

}  // namespace auth
}  // namespace services