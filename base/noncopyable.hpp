#ifndef RECIPES_SINGLETON_H
#define RECIPES_SINGLETON_H

namespace base {

class Noncopyable {
public:
  Noncopyable(const Noncopyable &) = delete;
  void operator=(const Noncopyable &) = delete;

protected:
  Noncopyable() = default;
  ~Noncopyable() = default;
};

} // namespace base

#endif // RECIPES_SINGLETON_H