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

#include "bitmap.hpp"

#include <fstream>
#include <stdexcept>
#include <filesystem>

using namespace testing;
using namespace icon_changer;

class BitmapTest : public Test {
protected:
	static void SetUpTestSuite() {
		const std::vector<std::string> requiered = {
			"data/valid_24bit.bmp",
			"data/invalid_header.bmp"
		};
		for (const auto& file : requiered) {
			ASSERT_TRUE(std::filesystem::exists(file)) 
				<< "Test BMP file does not exist.";
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
// TESTS
////////////////////////////////////////////////////////////////////////////////

TEST(BitmapTest, LoadValid24BitBMP) {
	bitmap bmp;
	ASSERT_TRUE(bmp.loadFromImage("data/valid_24bit.bmp"));

	EXPECT_EQ(bmp.getWidth(), 32);
	EXPECT_EQ(bmp.getHeight(), 32);
    EXPECT_EQ(bmp.getBitDepth(), 24);
    EXPECT_EQ(bmp.getPixels().size(), 32 * 32 * 3);
}

TEST(BitmapTest, LoadNonExistent_ThrowInvalidArgument) {
	bitmap bmp;
	EXPECT_THROW(bmp.loadFromImage("data/nonexistent.bmp"), std::invalid_argument);
}

TEST(BitmapTest, LoadInvalidHeader_ThrowRuntimeError) {
	bitmap bmp;
	EXPECT_THROW(bmp.loadFromImage("data/invalid_header.bmp"), std::runtime_error);
}

TEST(BitmapTest, SaveToIco_CreatesValidIco) {
	bitmap bmp;
	ASSERT_TRUE(bmp.loadFromImage("data/valid_24bit.bmp"));

	const std::string ico_path = "data/output_icon.ico";

	if (std::filesystem::exists(ico_path)) {
		std::filesystem::remove(ico_path);
	}

	ASSERT_TRUE(bmp.saveToIco(ico_path));
	ASSERT_TRUE(std::filesystem::exists(ico_path));

	std::ifstream ico(ico_path, std::ios::binary);
	ASSERT_TRUE(ico.is_open());
	uint16_t type, reserved, count;
	ico.read(reinterpret_cast<char*>(&reserved), 2); // idReserved
	ico.read(reinterpret_cast<char*>(&type), 2); // idType
	ico.read(reinterpret_cast<char*>(&count), 2); // idCount

	EXPECT_EQ(reserved, 0); 
	EXPECT_EQ(type, 1); // ICO type
	EXPECT_EQ(count, 1); // One image

	ico.close();
	std::filesystem::remove(ico_path);
}