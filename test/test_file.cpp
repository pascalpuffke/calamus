#include <gtest/gtest.h>
#include <util/files.h>

using namespace calamus;

static const auto path = std::filesystem::path { "../../LICENSE" };
static constexpr auto expected_size = 1500;

TEST(File, Size) {
    auto result = Files::file_size(path);
    EXPECT_TRUE(result.has_value());
    auto value = result.release_value();
    EXPECT_EQ(value, expected_size);
}

TEST(File, ReadFileAsT) {
    auto result = Files::read_file_as<u8>(path);
    EXPECT_TRUE(result.has_value());
    const auto value = result.release_value();
    EXPECT_EQ(value, 'B');
}

TEST(File, ReadFileAsString) {
    auto result = Files::read_file_as_string(path);
    EXPECT_TRUE(result.has_value());
    const auto value = result.release_value();
    EXPECT_FALSE(value.empty());
    EXPECT_EQ(value.at(0), 'B');
    EXPECT_EQ(value.size(), expected_size);
}

TEST(File, ReadFileAsLines) {
    auto result = Files::read_file_as_lines(path);
    EXPECT_TRUE(result.has_value());
    const auto value = result.release_value();
    EXPECT_FALSE(value.empty());
    EXPECT_EQ(value.size(), 28);
    const auto first_line = value.at(0);
    EXPECT_EQ(first_line, "BSD 3-Clause License");
}

TEST(File, ReadFileAsBytes) {
    auto result = Files::read_file_as_bytes(path);
    EXPECT_TRUE(result.has_value());
    const auto value = result.release_value();
    EXPECT_FALSE(value.empty());
    EXPECT_EQ(value.size(), expected_size);
    const auto first_byte = value.at(0);
    EXPECT_EQ(first_byte, 'B');
}

TEST(File, ReadLittleEndianFromStream) {
    auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
    EXPECT_TRUE(stream.is_open());
    auto result = Files::read_little_endian_from_stream<u16>(stream);
    EXPECT_TRUE(result.has_value());
    auto value = result.release_value();
    auto first_byte = value >> 8;
    auto second_byte = value & 0xFF;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    EXPECT_EQ(first_byte, 'S');
    EXPECT_EQ(second_byte, 'B');
#else
    EXPECT_EQ(first_byte, 'B');
    EXPECT_EQ(second_byte, 'S');
#endif
}

TEST(File, ReadBigEndianFromStream) {
    auto stream = std::ifstream { path, std::ios_base::in | std::ios_base::binary };
    EXPECT_TRUE(stream.is_open());
    auto result = Files::read_big_endian_from_stream<u16>(stream);
    EXPECT_TRUE(result.has_value());
    auto value = result.release_value();
    auto first_byte = value >> 8;
    auto second_byte = value & 0xFF;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    EXPECT_EQ(first_byte, 'B');
    EXPECT_EQ(second_byte, 'S');
#else
    EXPECT_EQ(first_byte, 'S');
    EXPECT_EQ(second_byte, 'B');
#endif
}
