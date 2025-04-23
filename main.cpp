
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <stdio.h>  // TODO: some setup for using modules

#include <nlohmann/json.hpp>

#include "httplib.h"
using json = nlohmann::json;

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "auth/auth.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main(void) {
  httplib::Server server;

  try {
    // Create an instance.
    mongocxx::instance inst{};
    const auto uri = mongocxx::uri{
        "mongodb+srv://"
        "edsonshivuri:7UgntlDCyRsn7ob4@cluster0.ljbvmqh.mongodb.net/"
        "?retryWrites=true&w=majority&appName=Cluster0"};

    // Set the version of the Stable API on the client
    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{
        mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);

    // Setup the connection and get a handle on the "admin" database.
    mongocxx::client conn{uri, client_options};

    auto collection = conn["testdb"]["testcollection"];

    collection.insert_one(make_document(kvp("test", "passed")));
  } catch (const std::exception &e) {
    // Handle errors
    std::cout << "Exception: " << e.what() << std::endl;
  }

  server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("<h1>Hello !!</h1>", "text/html");
  });

  server.Post("/", [](const httplib::Request &req, httplib::Response &res) {
    // handling client generatiion -> maybe ddoing that on a middleware layer
    httplib::Client client("https://dev-0nrio8oxyp8m8ddg.us.auth0.com");

    // Implement middleware such that this can be passed on
    auto body_raw = req.body;
    const json body_json = json::parse(body_raw);

    models::client::auth0::signup::request::RequestBody body =
        models::deserialize<
            models::client::auth0::signup::request::RequestBody>(body_json);
    json clientRequestBody =
        models::serialize<models::client::auth0::signup::request::RequestBody>(
            body);

    auto response = client.Post("/dbconnections/signup",
                                clientRequestBody.dump(), "application/json");

    if (response) {
      const int status = response.value().status;
      json body = json::parse(response.value().body);
      res.status = status;

      if (status != httplib::StatusCode::OK_200) {
        // maybe have an error middleware?
        res.set_content(body.dump(), "application/json");
        return;
      }

      // success
      auto user_auth = models::deserialize<
          models::client::auth0::signup::response::ResponseBody>(body);

      res.set_content(body.dump(), "text/json");

    } else {
      res.status = httplib::StatusCode::InternalServerError_500;
      json response_body;
      response_body["message"] = response.error();
      res.set_content(response_body.dump(), "application/json");
    }

    // res.set_content(, "text/json");
  });

  server.listen("0.0.0.0", 8080);

  return 0;
}
