#include <gtest/gtest.h>
#include <util/result.h>
#include <util/types.h>

using namespace calamus;

TEST(Result, ConstructWithValue) {
    constexpr const auto result = Result<i32> { 63014 };

    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(result.value(), 63014);
}

TEST(Result, ConstructWithError) {
    const auto result = Result<i32> {
        Error { "Some error message" }
    };

    EXPECT_TRUE(result.has_error());
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message(), "Some error message");
}

TEST(Result, ConstructWithFormattedError) {
    const auto result = Result<i32> {
        Error::formatted("Waiting for {} to happen?", "something")
    };

    EXPECT_TRUE(result.has_error());
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().message(), "Waiting for something to happen?");
}

TEST(Result, Match) {
    constexpr auto positive_result = Result<i32> { 1 };
    auto negative_result = Result<i32> { Error { "" } };

    auto success = false;
    positive_result.match(
        [&](auto value) { success = true; },
        [&](const auto& error) { success = false; }
    );
    EXPECT_TRUE(success);

    success = false;
    negative_result.match(
        [&](auto value) { success = false; },
        [&](const auto& error) { success = true; }
    );
    EXPECT_TRUE(success);
}

TEST(Result, ValueOr) {
    constexpr auto positive_result = Result<i32> { 2 };
    auto negative_result = Result<i32> { Error { "" } };

    auto value = positive_result.value_or(4);
    EXPECT_EQ(value, 2);

    value = negative_result.value_or(4);
    EXPECT_EQ(value, 4);
}

TEST(Result, ValueOrElse) {
    auto positive_result = Result<i32> { 2 };
    auto negative_result = Result<i32> { Error { "" } };

    auto value = positive_result.value_or_else([]() { return 4; });
    EXPECT_EQ(value, 2);

    value = negative_result.value_or_else([]() { return 4; });
    EXPECT_EQ(value, 4);
}

TEST(Result, ReleaseValue) {
    auto result = Result<std::string> {
        "According to all known laws of aviation, there is no way a bee should be able to fly."
    };

    const auto released = result.release_value();
    EXPECT_FALSE(result.has_value());
    EXPECT_FALSE(result.has_error());
    EXPECT_EQ(released, "According to all known laws of aviation, there is no way a bee should be able to fly.");
}

TEST(Result, TryMacro) {
    constexpr auto positive_result = Result<i32> { 2 };
    auto negative_result = Result<i32> { Error { "henlo" } };

    const auto some_function = [&]() -> Result<i32> {
        [[maybe_unused]] const auto value = TRY(positive_result);

        [[maybe_unused]] const auto return_here = TRY(negative_result);
        // The line above will cause an early return as negative_result holds an Error.

        return Result<i32> { 1 };
    };

    auto should_be_error = some_function();
    EXPECT_TRUE(should_be_error.has_error());
    EXPECT_EQ(should_be_error.error(), Error { "henlo" });

    // TRY releases (moves) positive values, but because we don't pass in a Result-returning expression,
    // it copies the Result internally (auto __try_result = (expr)) and our original positive_result is unaffected.
    EXPECT_TRUE(positive_result.has_value());
}