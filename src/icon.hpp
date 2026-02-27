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

#include <string_view>
#include <vector>

#include "ico_file.hpp"

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief Class to handle and manipulate icon (ICO, BMP) files.
/// \details This class allows for reading, extracting metadata and images,
/// as well as serializing the data back into PE resource format.
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
	std::vector<std::uint8_t>& get_header();

	///
	/// \brief Gets a reference to the image data of the icon file.
	/// \returns A vector of vectors of bytes, where each inner vector
	/// represents the data for one image.
	///
	std::vector<std::vector<std::uint8_t>>& get_images() noexcept;

private:
	///
	/// \brief Loads an ICO file and prepares it for use as a PE icon resource.
	/// \param file_path: Path to the ICO file.
	///
	void load_ico(std::string_view file_path);

	///
	/// \brief Loads a BMP file and converts it into a single-entry ICO resource.
	/// \param file_path: Path to the BMP file.
	///
	void load_bmp(std::string_view file_path);

	///
	/// \brief Converts a DIB header into an ICO directory entry.
	/// \param dib_image: Raw DIB image data.
	/// \returns A populated ICO directory entry corresponding to the DIB.
	///
	ico_file::entry dib_header_to_entry(std::vector<std::uint8_t>& dib_image);

private:
	///
	/// \brief The header of the ICO file for PE resource format.
	///
	std::vector<std::uint8_t> header;

	///
	/// \brief The image data for the ICO file.
	///
	std::vector<std::vector<std::uint8_t>> images;
};

} // namespace icon_changer
