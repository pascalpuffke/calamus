#include <gtest/gtest.h>
#include <ui/structs.h>

using namespace calamus;

TEST(Rectangle, ConstructionAndAccessors) {
    const auto a = IntRect { 0, 0, 16, 16 };
    const auto b = IntRect {
        IntPosition { 0, 0 },
        IntSize { 16, 16 },
    };

    EXPECT_EQ(a.x, 0);
    EXPECT_EQ(b.x, 0);
    EXPECT_EQ(a.y, 0);
    EXPECT_EQ(b.y, 0);
    EXPECT_EQ(a.width, 16);
    EXPECT_EQ(b.width, 16);
    EXPECT_EQ(a.height, 16);
    EXPECT_EQ(b.height, 16);
}

TEST(Rectangle, Addition) {
    const auto a = IntRect { 10, 10, 20, 20 };
    const auto b = a + a;

    EXPECT_EQ(b.x, 20);
    EXPECT_EQ(b.y, 20);
    EXPECT_EQ(b.width, 40);
    EXPECT_EQ(b.height, 40);
}

TEST(Rectangle, Subtraction) {
    const auto a = IntRect { 32, 32, 64, 64 };
    const auto b = IntRect { 16, 16, 32, 32 };
    const auto c = a - b;

    EXPECT_EQ(c.x, 16);
    EXPECT_EQ(c.y, 16);
    EXPECT_EQ(c.width, 32);
    EXPECT_EQ(c.height, 32);
}

TEST(Rectangle, Multiplication) {
    const auto a = IntRect { 16, 16, 16, 16 };
    const auto b = a * 2;

    EXPECT_EQ(b.x, 32);
    EXPECT_EQ(b.y, 32);
    EXPECT_EQ(b.width, 32);
    EXPECT_EQ(b.height, 32);
}

TEST(Rectangle, Division) {
    const auto a = IntRect { 16, 16, 16, 16 };
    const auto b = a / 2;

    EXPECT_EQ(b.x, 8);
    EXPECT_EQ(b.y, 8);
    EXPECT_EQ(b.width, 8);
    EXPECT_EQ(b.height, 8);
}

TEST(Rectangle, AdditionAssignment) {
    auto a = IntRect { 64, 64, 96, 128 };
    const auto b = IntRect { 16, 16, 16, 16};
    a += b;

    EXPECT_EQ(a.x, 80);
    EXPECT_EQ(a.y, 80);
    EXPECT_EQ(a.width, 112);
    EXPECT_EQ(a.height, 144);
}

TEST(Rectangle, SubtractionAssignment) {
    auto a = IntRect { 64, 64, 96, 128 };
    const auto b = IntRect { 16, 16, 16, 16};
    a -= b;

    EXPECT_EQ(a.x, 48);
    EXPECT_EQ(a.y, 48);
    EXPECT_EQ(a.width, 80);
    EXPECT_EQ(a.height, 112);
}


TEST(Rectangle, ToPosition) {
    const auto rect = IntRect { 16, 32, 1, 1 };
    const auto pos = rect.to_position();
    
    EXPECT_EQ(pos.x, 16);
    EXPECT_EQ(pos.y, 32);
}

TEST(Rectangle, ToSize) {
    const auto rect = IntRect { 0, 0, 32, 64 };
    const auto size = rect.to_size();

    EXPECT_EQ(size.width, 32);
    EXPECT_EQ(size.height, 64);
}

TEST(Rectangle, Equality) {
    const auto a = IntRect { 0, 0, 0, 0 };
    const auto b = IntRect { 16, 16, 16, 16 };

    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(a, IntRect());
}
