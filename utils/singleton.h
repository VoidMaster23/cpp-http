#pragma once
#include <memory>
#include <mutex>

namespace utils {

template <typename T>
class Singleton {
 private:
  static std::unique_ptr<T> instance;
  static std::mutex mutex;

 protected:
  Singleton() = default;
  ~Singleton() = default;

 public:
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

  template <typename... Args>
  static T& getInstance(Args&&... args) {
    std::lock_guard<std::mutex> lock(mutex);
    if (!instance) {
      instance = std::make_unique<T>(std::forward<Args>(args)...);
    }
    return *instance;
  }
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

template <typename T>
std::mutex Singleton<T>::mutex;

}  // namespace