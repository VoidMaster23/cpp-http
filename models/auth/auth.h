#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "modelBase.h"

namespace models {

// ================== request body ===================

namespace auth {

struct SignUpRequestBody {
  std::string user_name;
  std::string password;
  std::string email;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SignUpRequestBody, user_name, password, email)

}  // namespace auth

// ==================== response body

}  // namespace models