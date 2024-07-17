#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "Delegate.hpp"

// 参考https://github.com/chenshuo/recipes/blob/master/thread/test/SignalSlot_test.cc
// Delegate也会有委托方法空指针的问题。
TEST(DelegateTest, NonRegister) {
  recipes::Delegate<void()> delegate;
  bool isExecute = false;
  delegate.Publish();
  EXPECT_FALSE(isExecute);
}

TEST(DelegateTest, OneCallZeroArgument) {
  recipes::Delegate<void()> delegate;
  bool isExecute = false;
  delegate.Subscribe([&isExecute]() { isExecute = true; });
  delegate.Publish();
  EXPECT_TRUE(isExecute);
}

TEST(DelegateTest, OneCallOneArgument) {
  recipes::Delegate<void(int)> delegate;
  bool isExecute = false;
  delegate.Subscribe([&isExecute](int a) {
    isExecute = true;
    EXPECT_EQ(a, 11);
  });
  delegate.Publish(11);
  EXPECT_TRUE(isExecute);
}

TEST(DelegateTest, OneCallThreeArgument) {
  recipes::Delegate<void(int, double, std::string)> delegate;
  bool isExecute = false;
  delegate.Subscribe([&isExecute](int a, double b, std::string c) {
    isExecute = true;
    EXPECT_EQ(a, 11);
    EXPECT_EQ(b, 3.14);
    EXPECT_STREQ(c.c_str(), "qq start");
  });
  delegate.Publish(11, 3.14, "qq start");
  EXPECT_TRUE(isExecute);
}

// ===== DelegateThreadSafeTest =====
TEST(DelegateThreadSafeTest, NonRegister) {
  recipes::DelegateThreadSafe<void()> delegate;
  bool isExecute = false;
  delegate.Publish();
  EXPECT_FALSE(isExecute);
}

TEST(DelegateThreadSafeTest, OneCallZeroArgument) {
  recipes::DelegateThreadSafe<void()> delegate;
  bool isExecute = false;
  auto token = delegate.Subscribe([&isExecute]() { isExecute = true; });
  delegate.Publish();
  EXPECT_TRUE(isExecute);
}

TEST(DelegateThreadSafeTest, OneCallThreeArgument) {
  recipes::DelegateThreadSafe<void(int, double, std::string)> delegate;
  bool isExecute = false;
  auto token = delegate.Subscribe([&isExecute](int a, double b, std::string c) {
    isExecute = true;
    EXPECT_EQ(a, 11);
    EXPECT_EQ(b, 3.14);
    EXPECT_STREQ(c.c_str(), "qq start");
  });
  delegate.Publish(11, 3.14, "qq start");
  EXPECT_TRUE(isExecute);
}

TEST(DelegateThreadSafeTest, Life) {
  recipes::SubscribeTokenPtr s1;
  recipes::DelegateThreadSafe<void()> signal;
  int function1CallTimes = 0, function2CallTimes = 0;

  {
    s1 = signal.Subscribe([&function1CallTimes]() { ++function1CallTimes; });
    signal.Publish();

    s1 = signal.Subscribe([&function2CallTimes]() { ++function2CallTimes; });
    signal.Publish();
  }
  signal.Publish();

  EXPECT_EQ(function1CallTimes, 1);
  EXPECT_EQ(function2CallTimes, 2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}