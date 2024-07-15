#include <functional>
#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

#include "safe_observer.hpp"

int main() {

  std::cout << "hello\n";

  // 1.测试订阅和发布同步（单线程）执行
  recipes::SafeSignal<void(int, int)> signal1;
  auto f1 = [](int a, int b) {
    std::cout << a << std::endl;
    std::cout << b << std::endl;
  };
  auto f2 = [](int a, int b) { std::cout << a + b << std::endl; };
  auto token1 = signal1.Subscribe(f1);
  auto token2 = signal1.Subscribe(f2);
  int num1 = 11;
  signal1.Publish(11, 22);

  // 2.测试订阅和发布并行
  recipes::SafeSignal<void(int)> signal2;
  std::vector<recipes::SubscribeTokenPtr> tokens;
  auto f3 = [&](int id) {
    for (int i = 0; i < 500; i++) {
      tokens.push_back(
          signal2.Subscribe([i](int id) { std::cout << i << " "; }));
      if (i % 100 == 0) {
        std::cout << "start clear"<< std::endl;
        tokens.clear();
        std::cout << "end clear"<< std::endl;
      }
    }
  };
  auto f4 = [&](int num) {
    for (int i = 0; i < 50; i++) {
      std::cout << std::endl << i << " ====> pushlish" << std::endl;
      int t = i;
      signal2.Publish(std::move(t));
    }
  };
  std::function<void(int)> f5[] = {f3, f4};

  const int numThreads = 2;         // 指定要创建的线程数量
  std::vector<std::thread> threads; // 存储线程对象的容器

  // 创建并启动线程
  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back(f5[i % 5], i); // 使用 emplace_back 直接构造线程对象
  }

  for (auto &i : threads) {
    i.join();
  }

  return 0;
}