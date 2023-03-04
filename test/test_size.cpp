#include <gtest/gtest.h>
#include <ui/structs.h>

using namespace calamus;

TEST(Size, ConstructionAndAccessors) {
    constexpr const IntSize a = { 123, 456 };
    constexpr const IntSize b = IntSize { 10 }; // Explicit

    EXPECT_EQ(a.width, 123);
    EXPECT_EQ(a.height, 456);
    EXPECT_EQ(b.width, 10);
    EXPECT_EQ(b.height, 10);
}

TEST(Size, Addition) {
    constexpr const auto a = IntSize { 4, 10 };
    constexpr const auto b = IntSize { 6 };
    constexpr const auto c = a + b;

    EXPECT_EQ(c.width, 10);
    EXPECT_EQ(c.height, 16);
}

TEST(Size, Subtraction) {
    constexpr const auto a = IntSize { 4, 10 };
    constexpr const auto b = IntSize { 6 };
    constexpr const auto c = a - b;

    EXPECT_EQ(c.width, -2);
    EXPECT_EQ(c.height, 4);
}

TEST(Size, Multiplication) {
    constexpr const auto a = IntSize { 10, 20 };
    constexpr const auto b = a * 2;

    EXPECT_EQ(b.width, 20);
    EXPECT_EQ(b.height, 40);
}

TEST(Size, Division) {
    constexpr const auto a = IntSize { 10, 20 };
    constexpr const auto b = a / 2;

    EXPECT_EQ(b.width, 5);
    EXPECT_EQ(b.height, 10);
}

TEST(Size, AdditionAssignment) {
    auto a = IntSize { 100, 100 };
    constexpr const auto b = IntSize { 16, 32 };
    a += b;

    EXPECT_EQ(a.width, 116);
    EXPECT_EQ(a.height, 132);
}

TEST(Size, SubtractionAssignment) {
    auto a = IntSize { 116, 132 };
    constexpr const auto b = IntSize { 16, 32 };
    a -= b;

    EXPECT_EQ(a.width, 100);
    EXPECT_EQ(a.height, 100);
}

TEST(Size, ToPosition) {
    constexpr const auto size = IntSize { 16, 32 };
    constexpr const auto pos = size.to_position();

    EXPECT_EQ(pos.x, 16);
    EXPECT_EQ(pos.y, 32);
}

TEST(Size, Equality) {
    constexpr const auto a = IntSize { 16, 32 };
    constexpr const auto b = IntSize { 24, 48 };

    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(a, IntSize(16, 32));
}
