#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace models {

template <typename T>
concept Serializable = requires(T t, json j) {
  { to_json(j, t) } -> std::same_as<void>;
  { from_json(j, t) } -> std::same_as<void>;
};

template <Serializable T>
json serialize(const T model) {
  json j = model;

  return j;
};

template <Serializable T>
T deserialize(const json j) {
  return j.template get<T>();
};
};  // namespace models