#ifndef RECIPES_SAFE_OBSERVER_H_
#define RECIPES_SAFE_OBSERVER_H_

#include <assert.h>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "noncopyable.hpp"

// COW(copy on write)，保证：
// 1. 订阅和发布线程安全
// 2. 发布执行的方法，线程不安全，需要订阅者自己保证。
namespace recipes {

using SubscribeTokenPtr = std::shared_ptr<void>;

namespace detail {

template <typename Callback> struct SubscriberToken;

template <typename Callback> struct PublisherImpl : public noncopyable {
  using SubscribeTokenList =
      std::vector<std::weak_ptr<SubscriberToken<Callback>>>;

  PublisherImpl()
      : subscribe_token_list_ptr_(std::make_shared<SubscribeTokenList>()) {}

  void CopyOnWrite() {
    assert(mutex_.try_lock() == false);
    if (subscribe_token_list_ptr_.use_count() != 1) {
      subscribe_token_list_ptr_.reset(
          new SubscribeTokenList(*subscribe_token_list_ptr_));
    }
    assert(subscribe_token_list_ptr_.use_count() == 1);
  }

  void Clean() {
    std::lock_guard lock(mutex_);
    CopyOnWrite();
    SubscribeTokenList &list(*subscribe_token_list_ptr_);
    auto it = list.begin();
    while (it != list.end()) {
      if (it->expired()) {
        it = list.erase(it);
      } else {
        ++it;
      }
    }
  }

  std::mutex mutex_;
  std::shared_ptr<SubscribeTokenList> subscribe_token_list_ptr_;
};

template <typename Callback> struct SubscriberToken : public noncopyable {
  using Publisher = PublisherImpl<Callback>;
  SubscriberToken(const std::shared_ptr<Publisher> &publisher,
                  Callback &&callback)
      : publisher_(publisher), callback_(callback) {}

  ~SubscriberToken() {
    std::shared_ptr<Publisher> publisher(publisher_.lock());
    if (publisher) {
      publisher->Clean();
    }
  }

  std::weak_ptr<Publisher> publisher_;
  Callback callback_;
};

} // namespace detail

// 泛型类
template <typename Signature> class SafeSignal;

// 特化SafeSignal，为了符合函数直觉：SafeSignal<int(int,int)>
// RET仅为了模板匹配，实际忽略返回值
template <typename RET, typename... ARGS> class SafeSignal<RET(ARGS...)> {
public:
  using Callback = std::function<void(ARGS...)>;
  using PublisherImpl = detail::PublisherImpl<Callback>;
  using SubscriberToken = detail::SubscriberToken<Callback>;

  SafeSignal() : publisher_impl_(std::make_shared<PublisherImpl>()) {}

  ~SafeSignal() {}

  // COW(copy on write)
  SubscribeTokenPtr Subscribe(Callback &&func) {
    auto subscribe_token_ptr = std::make_shared<SubscriberToken>(
        publisher_impl_, std::forward<Callback>(func));
    Add(subscribe_token_ptr);
    return subscribe_token_ptr;
  }

  // copy on write保证原数组不被修改，而是去新建一个数组更新。
  // 所以某一时刻，读线程只需获取数组的指针即可，即临界区操作。
  // 尝试提升弱指针，成功就执行。
  void Publish(ARGS &&...args) {
    PublisherImpl &impl(*publisher_impl_);
    // 如果不加typename，这可能是一个静态成员。
    std::shared_ptr<typename PublisherImpl::SubscribeTokenList>
        subscribe_token_list_ptr;
    {
      std::lock_guard lock(impl.mutex_);
      subscribe_token_list_ptr = impl.subscribe_token_list_ptr_;
    }
    typename PublisherImpl::SubscribeTokenList &s(*subscribe_token_list_ptr);
    for (auto it = s.begin(); it != s.end(); ++it) {
      std::shared_ptr<SubscriberToken> observerImpl = it->lock();
      if (observerImpl)
        observerImpl->callback_(args...);
    }
  }

private:
  void Add(const std::shared_ptr<SubscriberToken> &subscribe_token_ptr) {
    PublisherImpl &impl(*publisher_impl_);
    {
      std::lock_guard lock(impl.mutex_);
      impl.CopyOnWrite();
      impl.subscribe_token_list_ptr_->push_back(subscribe_token_ptr);
    }
  }

  const std::shared_ptr<PublisherImpl> publisher_impl_;
};
} // namespace recipes
#endif // RECIPES_SAFE_OBSERVER_H_