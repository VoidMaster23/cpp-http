
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <stdio.h>  // TODO: some setup for using modules

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include "auth/auth.h"
#include "httplib.h"
#include "services/auth/authServices.hpp"
#include "utils/dbclient.h"

// try {
//   // Create an instance
//   auto c = utils::MongoPool::acquire();

//   auto collection = c["testdb"]["users"];

//   make_document()

//   collection.insert_one(make_document(kvp("test3", "passed")));
// } catch (const std::exception &e) {
//   // Handle errors
//   std::cout << "Exception: " << e.what() << std::endl;
// }

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main(void) {
  httplib::Server server;

  server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("<h1>Hello !!</h1>", "text/html");
  });

  server.Post("/", [](const httplib::Request &req, httplib::Response &res) {
    // Implement middleware such that this can be passed on
    auto body_raw = req.body;
    const json body_json = json::parse(body_raw);

    models::auth::SignUpRequestBody body =
        models::deserialize<models::auth::SignUpRequestBody>(body_json);

    auto [is_valid, error] = services::auth::validate_account(body);

    if (!is_valid && error) {
      res.status = httplib::BadRequest_400;

      json message = {"message", "error"};
      res.set_content(message, "application/json");
    }

    services::auth::handle_sign_up(body);

    res.set_content("ok", "text/json");
  });

  server.listen("0.0.0.0", 8080);

  return 0;
}
