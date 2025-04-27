
#pragma once
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <stdio.h>

#include "auth/auth.h"
#include "bcrypt/BCrypt.hpp"
#include "dbclient.h"
using json = nlohmann::json;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

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
void handle_sign_up(const models::auth::SignUpRequestBody body) {
  const std::string password_hash{generate_password_hash(body.password)};

  const models::auth::SignUpRequestBody user_account{
      .user_name = body.user_name,
      .password = password_hash,
      .email = body.email};

  auto collection = get_collection(CollectionType::Account);

  auto existing_record =
      collection.find_one(make_document(kvp("user_name", body.user_name)));

  // handle what happens when we exist
  if (!existing_record) {
    json values =
        models::serialize<models::auth::SignUpRequestBody>(user_account);

    collection.insert_one(bsoncxx::from_json(values.dump()));
  } else {
    std::cout << "NAH BRO HE EXISTS" << std::endl;
  }

  // todo: handle error
}

}  // namespace auth
}  // namespace services