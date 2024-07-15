#ifndef RECIPES_NONCOPYABLE_H_
#define RECIPES_NONCOPYABLE_H_

namespace recipes {

class noncopyable {
public:
  noncopyable(const noncopyable &) = delete;
  void operator=(const noncopyable &) = delete;

protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} // namespace recipes

#endif // RECIPES_NONCOPYABLE_H_