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

#include <fstream>
#include <string_view>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Macro to align structs to prevent padding.
/// \details This ensures that the struct's memory layout matches the layout of
/// packed binary data, such as when reading icon files in a specific format.
///
#define PACKED __attribute__((packed))


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief Class to handle and manipulate icon (ICO) files.
/// \details This class allows for reading, extracting metadata and images,
/// as well as serializing the data back into PE resource format.
/// \see https://en.wikipedia.org/wiki/ICO_(file_format)
///
class icon final
{
public:
	///
	/// \brief Constructor to initialize icon object from a file.
	/// \details Reads the ICO file, parses the header, entries, and images.
	/// \param file_path: The path to the ICO file to be loaded.
	///
	icon(std::string_view file_path);

	///
	/// \brief Gets the serialized header data for a PE icon resource.
	/// \details It follows the NEWHEADER and RESDIR format.
	/// \returns A vector of bytes representing the serialized header data.
	///
	std::vector<std::uint8_t> get_header() const;

	///
	/// \brief Gets a reference to the image data of the icon file.
	/// \returns A vector of vectors of bytes, where each inner vector
	/// represents the data for one image.
	///
	std::vector<std::vector<std::uint8_t>>& get_images() noexcept;

	/// \brief Creates an icon from a 24-bit BMP file
	/// \brief bmp_path: Path to the source BMP file
	/// \return icon object with one image
	static icon from_bmp(const std::string_view bmp_path);

private:
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
	struct PACKED icon_entry final
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

	///
	/// \brief This data structure corresponds to RESDIR for ICO files.
	///
	struct PACKED entry final
	{
		std::uint8_t  width;         ///< Image width in pixels, 0 means 256.
		std::uint8_t  height;        ///< Image height in pixels, 0 means 256.
		std::uint8_t  color_count;   ///< Number of colors in the color palette.
		std::uint8_t  reserved;      ///< Reserved byte, must be 0.
		std::uint16_t planes;        ///< Color planes, should be 0 or 1.
		std::uint16_t bit_count;     ///< Bits per pixel.
		std::uint32_t resource_size; ///< Size of the resource in bytes.
		std::uint16_t icon_id;       ///< Unique ordinal identifier of the RT_ICON resource.
	};

private:
	///
	/// \brief Opens the specified file and sets exceptions for failbit and badbit.
	/// \param file_path: The path to the file to be opened.
	/// \returns An input file stream for reading the file.
	///
	static std::ifstream open_file(std::string_view file_path);

	///
	/// \brief Serializes the header into a byte vector.
	/// \param header: The header structure to be serialized.
	/// \returns A vector of bytes representing the serialized header.
	///
	static std::vector<std::uint8_t> serialize(const header& header);

	///
	/// \brief Serializes an entry into a byte vector.
	/// \param entry: The entry structure to be serialized.
	/// \returns A vector of bytes representing the serialized entry.
	///
	static std::vector<std::uint8_t> serialize(const entry& entry);

	///
	/// \brief Reads the header of the ICO file and validates its content.
	/// \param file: The file to read from.
	///
	void read_header(std::ifstream& file);

	///
	/// \brief Reads the icon header and entries from the ICON file.
	/// \details The sanity check is not performed.
	/// \param file: The file to read from.
	/// \returns A vector of icon entries. Do not discard because it changes the
	/// file pointer.
	///
	[[nodiscard]] std::vector<icon_entry> read_icon_entries(std::ifstream& file);

	///
	/// \brief Reads the image data for each entry in the ICO file.
	/// \details It also checks the integrity of the metadata.
	/// \param file: The file to read from.
	/// \param entries: A list of icon entries containing metadata for each image.
	///
	void read_images(std::ifstream&                 file,
	                 const std::vector<icon_entry>& entries);

	///
	/// \brief Converts the icon entries into resource entries member.
	/// \param entries: The list of icon entries structures to be converted.
	///
	void convert_entries(const std::vector<icon_entry>& entries);

private:
	///
	/// \brief The header of the ICO file.
	/// \details Is used for PE resource without any changes.
	///
	header resource_header;

	///
	/// \brief The metadata for each image in the ICO file.
	/// \details Is actually stored as PE resource format.
	///
	std::vector<entry> resource_entries;

	///
	/// \brief The image data for the ICO file.
	///
	std::vector<std::vector<std::uint8_t>> images;
};

} // namespace icon_changer
