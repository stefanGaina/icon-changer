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
/// \brief Represents a BMP file.
/// \see https://en.wikipedia.org/wiki/BMP_file_format
///
class bmp_file final
{
public:
	///
	/// \brief This data structure corresponds to Bitmap file header.
	///
	struct PACKED header final
	{
		std::uint16_t type;         //< Type of the file.
		std::uint32_t file_size;    //< Size of the file in bytes.
		std::uint16_t reserved1;    //< Reserved, can be 0.
		std::uint16_t reserved2;    //< Reserved, can be 0.
		std::uint32_t image_offset; //< Offset of image data from the beginning of file.
	};

	///
	/// \brief This data structure corresponds to BITMAPINFOHEADER.
	///
	struct PACKED dib_header final
	{
		std::uint32_t header_size;           //< Size of the header in bytes.
		std::int32_t  width;                 //< Image width in pixels.
		std::int32_t  height;                //< Image height in pixels.
		std::uint16_t planes;                //< Number of color planes, must be 1.
		std::uint16_t bit_count;             //< Bits per pixel, typically: 1, 4, 8, 16, 24 and 32.
		std::uint32_t compression_method;    //< Compression method being used (e.g. BI_RGB).
		std::uint32_t image_size;            //< Size of the raw bitmap data.
		std::int32_t  horizontal_resolution; //< Horizontal resolution of the image, pixel per metre.
		std::int32_t  vertical_resolution;   //< Vertical resolution of the image, pixel per metre.
		std::uint32_t color_count;           //< Number of colors in the color palette, 0 means 2^n.
		std::uint32_t ignored;               //< Number of important colors used.
	};

	///
	/// \brief Reads the header and image data of an BMP file.
	/// \param file_path: Path to the BMP file.
	///
	bmp_file(std::string_view file_path);

	///
	/// \brief Gets the parsed BMP file header.
	/// \returns A copy of the BMP file header.
	///
	header get_header() const noexcept;

	///
	/// \brief Gets the raw BMP image data (DIB header and pixel array).
	/// \returns A reference to the image byte buffer.
	///
	std::vector<std::uint8_t>& get_image() noexcept;

private:
	///
	/// \brief Reads the header from the BMP file.
	/// \param file: The file to read from.
	///
	void read_header(std::ifstream& file);

	///
	/// \brief Reads the image data from BMP file.
	/// \param file: The file to read from.
	///
	void read_image(std::ifstream& file);

private:
	///
	/// \brief Parsed BMP file header.
	///
	header header_obj;

	///
	/// \brief Raw image data of the BMP file (DIB header and pixel array).
	///
	std::vector<std::uint8_t> image;
};

} // namespace icon_changer
