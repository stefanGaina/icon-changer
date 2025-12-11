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

#pragma once

////////////////////////////////////////////////////////////////////////////////
// HEADER FILE INCLUDES
////////////////////////////////////////////////////////////////////////////////

#include "utility.hpp"

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief TODO
/// \see https://en.wikipedia.org/wiki/BMP_file_format
///
class bmp_file final
{
public:
	struct PACKED header final
	{
		std::uint16_t type;
		std::uint32_t file_size;
		std::uint16_t reserved1;
		std::uint16_t reserved2;
		std::uint32_t image_offset;
	};

	// BITMAPINFOHEADER
	struct PACKED dib_header final
	{
		std::uint32_t header_size;
		std::int32_t  width;
		std::int32_t  height;
		std::uint16_t planes;
		std::uint16_t bit_count;
		std::uint32_t compression_method;
		std::uint32_t image_size;
		std::int32_t  horizontal_resolution;
		std::int32_t  vertical_resolution;
		std::uint32_t color_count;
		std::uint32_t ignored;
	};

	///
	/// \brief TODO
	///
	bmp_file(std::string_view file_path);

	header get_header() const noexcept;

	std::vector<std::uint8_t>& get_image() noexcept;

private:
	void read_header(std::ifstream& file);

	void read_image(std::ifstream& file);

private:
	header header_obj;

	std::vector<std::uint8_t> image;
};

} // namespace icon_changer
