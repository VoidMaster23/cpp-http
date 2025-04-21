#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "modelBase.h"

namespace models {
// ================== request body ===================
namespace server {
namespace signup {

struct SignUpBody {
  std::string user_name;
  std::string password;
  std::string email;
};

void to_json(json &j, const SignUpBody &s) {
  j = json{
      {"username", s.user_name}, {"password", s.password}, {"email", s.email}};
}

void from_json(const json &j, SignUpBody &s) {
  j.at("username").get_to(s.user_name);
  j.at("password").get_to(s.password);
  j.at("email").get_to(s.email);
}
}  // namespace signup
}  // namespace server

// ==================== response body

// ==================== CLIENT
namespace client {
namespace signup {

struct SignUpRequestBody : public server::signup::SignUpBody {
  std::string connection_id{"Username-Password-Authentication"};
  std::string client_id{"YdFzpiOcmSQsx5cA3b2cmStikN5HErb2"};
};

void to_json(json &j, const SignUpRequestBody &s) {
  server::signup::to_json(j,
                          static_cast<const server::signup::SignUpBody &>(s));

  j["connection"] = s.connection_id;
  j["client_id"] = s.client_id;
}

void from_json(const json &j, SignUpRequestBody &s) {
  from_json(j, static_cast<server::signup::SignUpBody &>(s));
}
}  // namespace signup

}  // namespace client

}  // namespace models
