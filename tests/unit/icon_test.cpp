#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "icon.cpp"

using namespace testing;
using namespace icon_changer;

TEST(icon, constructor_open_fail)
{
	static constexpr std::string_view INVALID_ICON_PATH = "invalid.ico";

	EXPECT_THAT([]() {
		icon icon = { INVALID_ICON_PATH };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Failed to open \"{}\"!", INVALID_ICON_PATH))));
}

TEST(icon, constructor_header_reserved_fail)
{
	EXPECT_THAT([]() {
		icon icon = { std::string{ TEST_DATA_PATH } + "header_reserved_ffff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Header reserved bytes are 0x{:X}, expecting 0!", 0xFFFF))));
}

TEST(icon, constructor_header_cur_fail)
{
	EXPECT_THAT([]() {
		icon icon = { std::string{ TEST_DATA_PATH } + "header_cur.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr("Image is of CUR type, not ICO!")));
}

TEST(icon, constructor_header_type_fail)
{
	EXPECT_THAT([]() {
		icon icon = { std::string{ TEST_DATA_PATH } + "header_type_ffff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Image type 0x{:X} is invalid!", 0xFFFF))));
}
