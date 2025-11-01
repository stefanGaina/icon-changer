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

#include "icon_mock.hpp"
#include "icon_changer.cpp"

#include <stdexcept>

using namespace testing;
using namespace icon_changer;

////////////////////////////////////////////////////////////////////////////////
// TESTS
////////////////////////////////////////////////////////////////////////////////

TEST(icon_changer, change_icon_cli_version_success)
{
	const char* arguments[] = { "icon-changer.exe", "--version" };

	change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
}

TEST(icon_changer, change_icon_cli_v_success)
{
	const char* arguments[] = { "icon-changer.exe", "-v" };

	change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
}

TEST(icon_changer, change_icon_cli_1_parameter_missing_fail)
{
	const char* arguments[] = { "icon-changer.exe", "a.ico" };

	ASSERT_THAT([&]()
	{
		change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("1 parameter(s) missing!")));
}

TEST(icon_changer, change_icon_cli_2_parameters_missing_fail)
{
	const char* arguments[] = { "icon-changer.exe" };

	ASSERT_THAT([&]()
	{
		change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("2 parameter(s) missing!")));
}

TEST(icon_changer, change_icon_cli_inexistent_ico_fail)
{
	static constexpr std::string_view ICON_PATH = "inexistent.ico";

	const char* arguments[] = { "icon-changer.exe", ICON_PATH.data(), "a.exe", "extra" };

	ASSERT_THAT([&]()
	{
		change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("\"{}\" does not exist!", ICON_PATH))));
}

TEST(icon_changer, change_icon_cli_inexistent_exe_fail)
{
	static constexpr std::string_view EXE_PATH = "inexistent.exe";

	const std::string icon_path   = std::string{ TEST_DATA_PATH } + "image1.ico";
	const char*       arguments[] = { "icon-changer.exe", icon_path.c_str(), EXE_PATH.data() };

	ASSERT_THAT([&]()
	{
		change_icon_cli(sizeof(arguments) / sizeof(arguments[0]), arguments);
	},
	ThrowsMessage<std::invalid_argument>(HasSubstr(std::format("\"{}\" does not exist!", EXE_PATH))));
}

TEST(icon_changer, change_icon_gui)
{
	ASSERT_THAT([]()
	{
		change_icon_gui();
	},
	ThrowsMessage<std::runtime_error>(HasSubstr("GUI not yet implemented!")));
}

TEST(icon_changer_utils, verify_suppoted_image_valid) {
	EXPECT_NO_THROW(verify_supported_image("icon.ico"));
	EXPECT_NO_THROW(verify_supported_image("bitmap.bmp"));
	EXPECT_NO_THROW(verify_supported_image("image.png"));
	EXPECT_NO_THROW(verify_supported_image("ICON.ICO"));
}

TEST(icon_changer_utils, verify_supported_image_notvalid) {
	EXPECT_THROW(verify_supported_image("file.txt"), std::invalid_argument);
	EXPECT_THROW(verify_supported_image("image.jpeg"), std::invalid_argument);
	EXPECT_THROW(verify_supported_image("photo.jpg"), std::invalid_argument);
}
TEST(icon_changer_utils, get_image_type_recognizes_formats) {
	EXPECT_EQ(get_image_type("icon.ico"), "ico");
	EXPECT_EQ(get_image_type("image.jpg"), "");
	EXPECT_EQ(get_image_type("bitmap.bmp"), "bmp");
	EXPECT_EQ(get_image_type("unknow.jpeg"), "");
	EXPECT_EQ(get_image_type("doc.txt"), "");
	EXPECT_EQ(get_image_type("strange.FILE.PNG"), "png"); // checking lowercase work properly
	EXPECT_EQ(get_image_type("no_extension"), "");
}
TEST(icon_changer_utils, prepare_icon_existing_ico) {
	const std::string path = std::string(TEST_DATA_PATH) + "image1.ico";
	EXPECT_EQ(prepare_icon(path), path);
}
TEST(icon_changer_utils, prepare_icon_unsupported_file) {
	const std::string path = std::string(TEST_DATA_PATH) + "image1.jpeg";
	EXPECT_THROW(prepare_icon(path), std::invalid_argument);
}