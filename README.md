# cpp-recipes
C++菜谱（字典），记录代码案例。
# 环境
- clang
- ubuntu server 24
- c++17

[TOC]

# 目录
# 1 禁止复制
1. five rule，编译器对于`class`默认生成5个函数：
    - 拷贝和移动构造
    - 拷贝和移动赋值
    - 析构函数

    如果没定义「构造函数」还会生成「默认构造函数」。

2. 如果删除「初复制始化」，那么「移动初始化」也会被删除，反之亦然。
3. 同理「=赋值重载」和「移动赋值重载」。

```c++
class Noncopyable {
public:
  Noncopyable(const Noncopyable &) = delete;
  void operator=(const Noncopyable &) = delete;

  // 这样也行
  // Noncopyable(const Noncopyable &&) = delete;
  // void operator=(const Noncopyable &&) = delete;
protected:
  // 禁止直接实例化该类
  Noncopyable() = default;
  // 没有任何理由 Noncopyable* base = new NoncopyableDerived();
  ~Noncopyable() = default;
};
```
# 2 单例模式
## 2.1 线程安全的懒加载
使用模板方式实现，因为原来的类任然可以实例化，建议把模板改成每个类自身实现。
### 2.1.1 实现代码模板
```C++
class A : Noncopyable {
public:
  // ...
  static A& instance() {
    static A instance;
    return instance;
  }
  // ...
protected:
  A() = default;
  ~A() = default;
};
```

### 2.1.2 模板
- c++11之前
```c++
#include <pthread.h>

template<typename T>
class Singleton : Noncopyable
{
 public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T& instance()
  {
    pthread_once(&ponce_, &Singleton::init);
    assert(value_ != NULL);
    return *value_;
  }
 private:
  static pthread_once_t ponce_;
  static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;
```

- c++11及之后
```C++
template <typename T> class Singleton : Noncopyable {
public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T& instance() {
    static T instance;
    return instance;
  }
};
```

# 3 委托
简易版C# delegate

使用例子：
```c++
// 线程不安全
// 没有注销功能
recipes::Delegate<void()> delegate;
delegate.Subscribe([]() { cout << "hello\n"; });
delegate.Publish();

// 线程安全
// 智能指针控制生命周期
recipes::DelegateThreadSafe<void()> delegate;
auto token = delegate.Subscribe([]() { cout << "hello\n"; });
delegate.Publish();
```