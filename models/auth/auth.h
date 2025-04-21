#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "modelBase.h"

namespace models {

// ================== request body ===================
namespace server {
namespace signup {

struct SignUpBody {
  std::string username;
  std::string password;
  std::string email;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SignUpBody, username, password, email)

}  // namespace signup
}  // namespace server

// ==================== response body

// ==================== CLIENT
namespace client {

namespace auth0 {
namespace signup {
namespace request {

struct RequestBody : public server::signup::SignUpBody {
  std::string connection_id{"Username-Password-Authentication"};
  std::string client_id{"YdFzpiOcmSQsx5cA3b2cmStikN5HErb2"};
};

inline void to_json(json &j, const RequestBody &s) {
  j = static_cast<json>(static_cast<const server::signup::SignUpBody &>(s));
  j["connection"] = s.connection_id;
  j["client_id"] = s.client_id;
}

inline void from_json(const json &j, RequestBody &s) {
  j.get_to<server::signup::SignUpBody>(s);  // parse base
}
}  // namespace request

namespace response {
struct ResponseBody {
  std::string _id;
  bool email_verified;
  std::string email;
  std::string username;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ResponseBody, _id, email, username , email_verified)
}  // namespace response

}  // namespace signup
}  // namespace auth0

}  // namespace client

}  // namespace models