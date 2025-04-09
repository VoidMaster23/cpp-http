
#include "httplib.h"

int main(void) {
  httplib::Server server;

  server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
    res.set_content("Hello !!", "text/plain");
  });

  server.listen("localhost", 8080);

  return 0;
}
