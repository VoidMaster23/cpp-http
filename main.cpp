
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <stdio.h>  // TODO: some setup for using modules

#include <nlohmann/json.hpp>

#include "httplib.h"
using json = nlohmann::json;

#include "auth/auth.h"

int main(void) {
  httplib::Server server;

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
