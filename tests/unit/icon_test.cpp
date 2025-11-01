////////////////////////////////////////////////////////////////////////////////
// This is free and unencumbered software released into the public domain.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to https://unlicense.org
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// HEADER FILE INCLUDES
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "icon.cpp"

#include <stdexcept>

using namespace testing;
using namespace icon_changer;

////////////////////////////////////////////////////////////////////////////////
// TESTS
////////////////////////////////////////////////////////////////////////////////

TEST(icon, constructor_open_fail)
{
	static constexpr std::string_view INVALID_ICON_PATH = "invalid.ico";

	ASSERT_THAT([]()
	{
		icon icon = { INVALID_ICON_PATH };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Failed to open \"{}\"!", INVALID_ICON_PATH))));
}

TEST(icon, constructor_header_read_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "header_incomplete.ico" };
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("Failed to read icon header from file.")));
}

TEST(icon, constructor_header_reserved_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "header_reserved_ffff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Header reserved bytes are 0x{:X}, expecting 0x{:X}!", 0xFFFF, 0x0000))));
}

TEST(icon, constructor_header_cur_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "header_cur.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr("Image is of CUR type, not ICO!")));
}

TEST(icon, constructor_header_type_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "header_type_ffff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Image type 0x{:X} is invalid!", 0xFFFF))));
}

TEST(icon, constructor_header_count_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "header_count_0.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr("Icon does not have image entries!")));
}

TEST(icon, constructor_entry_read_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "entry_incomplete.ico" };
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("Failed to read icon entry data from file.")));
}

TEST(icon, constructor_entry_reserved_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "entry_reserved_ff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Entry's reserved byte is 0x{:X}, excepting 0x{:X}!", 0xFF, 0x00))));
}

TEST(icon, constructor_entry_planes_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "entry_planes_ffff.ico" };
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("Entry's color planes is 0x{:X}, expecting 0x{:X} or 0x{:X}!", 0xFFFF, 0x0000, 0x0001))));
}

TEST(icon, constructor_image_incomplete_fail)
{
	ASSERT_THAT([]()
	{
		icon icon = { std::string{ TEST_DATA_PATH } + "image_incomplete.ico" };
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("Failed to read icon image data from file.")));
}

TEST(icon, get_success)
{
	icon                                    icon            = { std::string{ TEST_DATA_PATH } + "image1.ico" };
	const std::vector<std::uint8_t>         header          = icon.get_header();
	std::vector<std::vector<std::uint8_t>>& images          = icon.get_images();
	const std::vector<std::uint8_t>         expected_header = { 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x20, 0x20, 0x00, 0x00, 0x01, 0x00,
																0x20, 0x00, 0xA8, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 };

	EXPECT_EQ(22, header.size());
	EXPECT_EQ(expected_header, header);

	EXPECT_EQ(1, images.size());
	EXPECT_EQ(0x10A8, images.front().size());
	// TODO: check the content of the image
}
