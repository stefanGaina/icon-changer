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

#include <vector>

#include "utility.hpp"

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief TODO
///
class ico_file final
{
public:
	///
	/// \brief This data structure corresponds to ICONDIR.
	/// \details It also corresponds to NEWHEADER, because it's the same.
	///
	struct PACKED header final
	{
		std::uint16_t reserved;      ///< Reserved 2 bytes, must be 0.
		std::uint16_t type;          ///< Image type: 1 - ICO, 2 - CUR, other values are invalid.
		std::uint16_t entries_count; ///< Number of images in the file.
	};

	///
	/// \brief This data structure corresponds to ICONDIRENTRY.
	///
	struct PACKED entry final
	{
		std::uint8_t  width;        ///< Image width in pixels, 0 means 256.
		std::uint8_t  height;       ///< Image height in pixels, 0 means 256.
		std::uint8_t  color_count;  ///< Number of colors in the color palette.
		std::uint8_t  reserved;     ///< Reserved byte, must be 0.
		std::uint16_t planes;       ///< In ICO format: color planes, 0 or 1.
		std::uint16_t bit_count;    ///< In ICO format: bits per pixel.
		std::uint32_t image_size;   ///< Image data size in bytes.
		std::uint32_t image_offset; ///< Offset of image data from the beginning of file.
	};

public:
	///
	/// \brief TODO
	///
	ico_file(std::string_view file_path);

	///
	/// \brief TODO
	///
	header get_header() const noexcept;

	///
	/// \brief TODO
	///
	std::vector<entry>& get_entries() noexcept;

	///
	/// \brief TODO
	///
	std::vector<std::vector<std::uint8_t>>& get_images() noexcept;

private:
	///
	/// \brief TODO
	///
	[[nodiscard]] static std::vector<std::uint8_t> read_image(std::ifstream& file,
	                                                          std::uint32_t  size);

	///
	/// \brief Reads the header of the ICO file and validates its content.
	/// \param file: The file to read from.
	///
	void read_header(std::ifstream& file);

	///
	/// \brief Reads the icon header and entries from the ICO file.
	/// \details The sanity check is not performed.
	/// \param file: The file to read from.
	///
	void read_entries(std::ifstream& file);

	///
	/// \brief Reads the image data for each entry in the ICO file.
	/// \details It also checks the integrity of the metadata.
	/// \param file: The file to read from.
	///
	void read_images(std::ifstream& file);

private:
	///
	/// \brief The header of the ICO file.
	/// \details Is used for PE resource without any changes.
	///
	header header_obj;

	///
	/// \brief The metadata for each image in the ICO file.
	/// \details Is actually stored as PE resource format.
	///
	std::vector<entry> entries;

	///
	/// \brief The image data for the ICO file.
	///
	std::vector<std::vector<std::uint8_t>> images;
};

} // namespace icon_changer
