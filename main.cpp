
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
    // TODO: abstract the fuck out of this
    // TODO: think about how best to abstract this, thinking a middleware but
    // idk bro, this is also the first one so I'll see how it goes
    auto body_raw = req.body;
    const json body_json = json::parse(body_raw);

    models::SignUpBody body = models::deserialize<models::SignUpBody>(body_json);

    std::cout << body.user_name << std::endl;

    json response = {{"message", "ok"}};

    res.set_content(response.dump(), "text/json");
  });

  server.listen("0.0.0.0", 8080);

  return 0;
}
