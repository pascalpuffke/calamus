#include <gtest/gtest.h>
#include <ui/structs.h>

using namespace calamus;

TEST(BasicColor, ConstructParts) {
    auto color = BasicColor<u8>::from_parts(0xDE, 0xAD, 0xBE);

    EXPECT_EQ(color.r, 0xDE);
    EXPECT_EQ(color.g, 0xAD);
    EXPECT_EQ(color.b, 0xBE);
    EXPECT_EQ(color.a, 0xFF);
}

TEST(BasicColor, ConstructPartsWithAlpha) {
    auto color = BasicColor<u8>::from_parts(0xDE, 0xAD, 0xBE, 0xEF);

    EXPECT_EQ(color.r, 0xDE);
    EXPECT_EQ(color.g, 0xAD);
    EXPECT_EQ(color.b, 0xBE);
    EXPECT_EQ(color.a, 0xEF);
}

TEST(BasicColor, ConstructHex) {
    auto color = BasicColor<u8>::from_hex(0xDEADBE);

    EXPECT_EQ(color.r, 0xDE);
    EXPECT_EQ(color.g, 0xAD);
    EXPECT_EQ(color.b, 0xBE);
    EXPECT_EQ(color.a, 0xFF);
}

TEST(BasicColor, ConstructHexWithAlpha) {
    auto color = BasicColor<u8>::from_hex(0xDEADBEEF);

    EXPECT_EQ(color.r, 0xAD);
    EXPECT_EQ(color.g, 0xBE);
    EXPECT_EQ(color.b, 0xEF);
    EXPECT_EQ(color.a, 0xDE);
}

TEST(BasicColor, Addition) {
    const auto a = BasicColor<u8>::from_parts(100, 100, 100, 0);
    const auto b = BasicColor<u8>::from_parts(255, 0, 42);
    const auto c = a + b;

    EXPECT_EQ(c.r, 255);
    EXPECT_EQ(c.g, 100);
    EXPECT_EQ(c.b, 142);
    EXPECT_EQ(c.a, 255);
}

TEST(BasicColor, Subtraction) {
    const auto a = BasicColor<u8>::from_parts(100, 100, 100);
    const auto b = BasicColor<u8>::from_parts(255, 0, 42);
    const auto c = a - b;

    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 100);
    EXPECT_EQ(c.b, 58);
    EXPECT_EQ(c.a, 0);
}

TEST(BasicColor, AdditionAssignment) {
    auto a = BasicColor<u8>::from_parts(100, 100, 100, 0);
    const auto b = BasicColor<u8>::from_parts(255, 0, 42);
    a += b;

    EXPECT_EQ(a.r, 255);
    EXPECT_EQ(a.g, 100);
    EXPECT_EQ(a.b, 142);
    EXPECT_EQ(a.a, 255);
}

TEST(BasicColor, SubtractionAssignment) {
    auto a = BasicColor<u8>::from_parts(100, 100, 100);
    const auto b = BasicColor<u8>::from_parts(255, 0, 42);
    a -= b;

    EXPECT_EQ(a.r, 0);
    EXPECT_EQ(a.g, 100);
    EXPECT_EQ(a.b, 58);
    EXPECT_EQ(a.a, 0);
}

TEST(BasicColor, Equality) {
    const auto a = BasicColor<u8>::from_hex(0);
    const auto b = BasicColor<u8>::from_hex(0xFFFFFFFF);

    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
    EXPECT_NE(a, b);
    EXPECT_EQ(a, BasicColor<u8>::from_parts(0, 0, 0, 0xFF));
}
