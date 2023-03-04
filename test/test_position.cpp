#include <gtest/gtest.h>
#include <ui/structs.h>

using namespace calamus;

TEST(Position, ConstructionAndAccessors) {
    constexpr const IntPosition a = { 123, 456 };
    constexpr const IntPosition b = IntPosition { 10 }; // Explicit

    EXPECT_EQ(a.x, 123);
    EXPECT_EQ(a.y, 456);
    EXPECT_EQ(b.x, 10);
    EXPECT_EQ(b.y, 10);
}

TEST(Position, Addition) {
    constexpr const auto a = IntPosition { 4, 10 };
    constexpr const auto b = IntPosition { 6 };
    constexpr const auto c = a + b;

    EXPECT_EQ(c.x, 10);
    EXPECT_EQ(c.y, 16);
}

TEST(Position, Subtraction) {
    constexpr const auto a = IntPosition { 4, 10 };
    constexpr const auto b = IntPosition { 6 };
    constexpr const auto c = a - b;

    EXPECT_EQ(c.x, -2);
    EXPECT_EQ(c.y, 4);
}

TEST(Position, Multiplication) {
    constexpr const auto a = IntPosition { 10, 20 };
    constexpr const auto b = a * 2;

    EXPECT_EQ(b.x, 20);
    EXPECT_EQ(b.y, 40);
}

TEST(Position, Division) {
    constexpr const auto a = IntPosition { 10, 20 };
    constexpr const auto b = a / 2;

    EXPECT_EQ(b.x, 5);
    EXPECT_EQ(b.y, 10);
}

TEST(Position, AdditionAssignment) {
    auto a = IntPosition { 100, 100 };
    constexpr const auto b = IntPosition { 16, 32 };
    a += b;

    EXPECT_EQ(a.x, 116);
    EXPECT_EQ(a.y, 132);
}

TEST(Position, SubtractionAssignment) {
    auto a = IntPosition { 116, 132 };
    constexpr const auto b = IntPosition { 16, 32 };
    a -= b;

    EXPECT_EQ(a.x, 100);
    EXPECT_EQ(a.y, 100);
}

TEST(Position, Equality) {
    constexpr const auto a = IntPosition { 16, 32 };
    constexpr const auto b = IntPosition { 24, 48 };

    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(a, IntPosition(16, 32));
}
