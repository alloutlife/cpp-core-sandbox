#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>

class A {
  public:
    bool b_{false};
    char ch_{'a'};
    unsigned long long u64_{19};
};

class B {
  public:
  private:
    bool b_{false};
    unsigned long long u64_{19};
    char ch_{'a'};
};

class C {
  public:
    unsigned long long u64_{19};
    bool b_{false};
    char ch_{'a'};
};

TEST(allocations, class_members_order_matters) {
    EXPECT_LT(sizeof(A), sizeof(B));
    EXPECT_EQ(sizeof(A), sizeof(C));
    std::cout << "size of 'A' is: " << sizeof(A) << std::endl;
    std::cout << "size of 'B' is: " << sizeof(B) << std::endl;
    std::cout << "size of 'C' is: " << sizeof(C) << std::endl;
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_THAT("hello", ::testing::StrNe("world"));
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}