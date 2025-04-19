#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "modelBase.h"

namespace models {
// ================== request body ===================
struct SignUpBody {
  std::string user_name;
  std::string password;
  std::string email;
};

void to_json(json &j, const SignUpBody &s) {
  j = json{
      {"user_name", s.user_name}, {"password", s.password}, {"email", s.email}};
}

void from_json(const json &j, SignUpBody &s) {
  j.at("user_name").get_to(s.user_name);
  j.at("password").get_to(s.password);
  j.at("email").get_to(s.email);
}


// ==================== response body
}  // namespace models
