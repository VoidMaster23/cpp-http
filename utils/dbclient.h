#pragma once

#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "singleton.h"

namespace utils {

class MongoPool {
 public:
  static mongocxx::pool& get() {
    return utils::Singleton<mongocxx::pool>::getInstance(
        mongocxx::uri{"mongodb+srv://"
                      "edsonshivuri:7UgntlDCyRsn7ob4@cluster0.ljbvmqh.mongodb."
                      "net/?retryWrites=true&w=majority&appName=Cluster0"});
  }

  static mongocxx::pool::entry acquire() { return get().acquire(); }
};

}  // namespace utils