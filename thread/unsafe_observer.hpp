#ifndef RECIPES_UNSAFE_OBSERVER_H_
#define RECIPES_UNSAFE_OBSERVER_H_

#include <functional>
#include <vector>

namespace recipes {

template <typename Signature> class UnsafeSignal;

template <typename RET, typename... ARGS> class UnsafeSignal<RET(ARGS...)> {
public:
  typedef std::function<void(ARGS...)> Functor;

  void connect(Functor &&func) {
    functors_.push_back(std::forward<Functor>(func));
  }

  void call(ARGS &&...args) {
    for (auto f : functors_) {
      f(args...);
    }
  }

private:
  std::vector<Functor> functors_;
};
} // namespace recipes
#endif // RECIPES_UNSAFE_OBSERVER_H_