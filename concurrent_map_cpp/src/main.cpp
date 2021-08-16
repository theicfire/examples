#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <utility>

template <typename K, typename V>
class ConcurrentMap {
  using find_type = std::optional<V>;

 public:
  // We use a function template here to make M&& a "forward type aka universal
  // type". This is helpful because we can insert both rvalues and lvalues.
  template <typename M>
  bool insert(const K& k, M&& v) {
    const std::lock_guard g(mut_);
    auto ret = m_.insert_or_assign(k, std::forward<M>(v));
    return ret.second;
  }

  void erase(const K& k) {
    const std::lock_guard g(mut_);
    m_.erase(k);
  }

  int size() {
    const std::lock_guard g(mut_);
    return m_.size();
  }

  find_type find_copy(const K& k) {
    const std::lock_guard g(mut_);
    auto it = m_.find(k);
    if (it != m_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  bool contains(const K& k) {
    const std::lock_guard g(mut_);
    return m_.count(k) > 0;
  }

  void clear() {
    const std::lock_guard g(mut_);
    return m_.clear();
  }

  // I didn't define find_reference because that could create race conditions --
  // editing some reference at the same time.
  bool find_and(const K& k, const std::function<void(V&)>& f) {
    const std::lock_guard g(mut_);
    auto it = m_.find(k);
    if (it != m_.end()) {
      f(it->second);
      return true;
    }
    return false;
  }

  // Note that this does not return a copy, but moves the type. So remove() is
  // possible with move-only types.
  find_type remove(const K& k) {
    const std::lock_guard g(mut_);
    auto it = m_.find(k);
    if (it != m_.end()) {
      auto ret = std::move(it->second);
      m_.erase(it);
      return ret;
    }
    return std::nullopt;
  }

  find_type remove_if(const K& k, const std::function<bool(const V&)>& f) {
    const std::lock_guard g(mut_);
    auto it = m_.find(k);
    if (it != m_.end()) {
      if (f(it->second)) {
        auto ret = std::move(it->second);
        m_.erase(it);
        return ret;
      }
    }
    return std::nullopt;
  }

 private:
  std::map<K, V> m_;
  std::mutex mut_;
};

class Holder {
 public:
  Holder(int x) : x_(x) {}
  // Holder ( Holder && ) = default;
  // Holder &  operator= ( Holder && ) = default;
  // Holder ( const Holder & ) = delete;
  // Holder & operator= ( const Holder & ) = delete;

  int x_;
};

void use_int() {
  using Value = int;
  ConcurrentMap<int, Value> m;
  int thing = 4;
  m.insert(3, thing);
  // TODO I'm not sure why I can't edit "thing" here and have the changes not
  // show up. From what I can tell, insert_or_assign will take the l-value,
  // right? Seems like it still makes a copy though?
  thing = 10;
  m.insert(4, 4);
  m.insert(5, 4);

  printf("m size %d\n", m.size());
  m.find_and(4, [](Value& x) { x = 6; });
  m.find_and(3, [](Value& x) { printf("m[3] =  %d\n", x); });
  m.find_and(4, [](Value& x) { printf("m[4] =  %d\n", x); });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
  m.remove_if(4, [](const Value& v) { return v < 10; });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
}

void use_unique() {
  // unique_ptr is a move-only type, so I'm showing ConcurrentMap is useable
  // with this.
  using Value = std::unique_ptr<int>;
  ConcurrentMap<int, Value> m;
  auto thing = std::make_unique<int>(4);
  m.insert(3, std::move(thing));
  m.insert(4, std::make_unique<int>(4));
  m.insert(5, std::make_unique<int>(4));

  printf("m size %d\n", m.size());
  m.find_and(4, [](Value& x) { *x = 6; });
  m.find_and(3, [](Value& x) { printf("m[3] =  %d\n", *x); });
  m.find_and(4, [](Value& x) { printf("m[4] =  %d\n", *x); });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
  m.remove_if(4, [](const Value& v) { return *v < 10; });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
}

void use_shared() {
  using Value = std::shared_ptr<int>;
  ConcurrentMap<int, Value> m;
  auto thing = std::make_shared<int>(4);
  m.insert(3, thing);
  *thing = 10;
  m.insert(4, std::make_shared<int>(4));
  m.insert(5, std::make_shared<int>(4));

  printf("m size %d\n", m.size());
  m.find_and(4, [](Value& x) { *x = 6; });
  m.find_and(3, [](Value& x) { printf("m[3] =  %d\n", *x); });
  m.find_and(4, [](Value& x) { printf("m[4] =  %d\n", *x); });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
  m.remove_if(4, [](const Value& v) { return *v < 10; });
  printf("m[4] exists?  %s\n", m.contains(4) ? "yes" : "no");
}

int main() {
  use_int();
  use_unique();
  use_shared();
  return 0;
}
