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

#include "ico_file.hpp"

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

ico_file::ico_file(const std::string_view file_path)
    : header_obj{}
    , entries{}
    , images{}
{
	std::ifstream file = open_file(file_path);

	read_header(file);
	read_entries(file);
	read_images(file);
}

ico_file::header ico_file::get_header() const noexcept
{
	return header_obj;
}

std::vector<ico_file::entry>& ico_file::get_entries() noexcept
{
	return entries;
}

std::vector<std::vector<std::uint8_t>>& ico_file::get_images() noexcept
{
	return images;
}

std::vector<std::uint8_t> ico_file::read_image(std::ifstream&      file,
                                               const std::uint32_t size)
{
	std::vector<std::uint8_t> image = {};

	image.resize(size);

	try
	{
		file.read(reinterpret_cast<char*>(image.data()), image.size());
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error{ std::format("Failed to read {} bytes from ICO image!", image.size()) };
	}

	return image;
}

void ico_file::read_header(std::ifstream& file)
{
	static constexpr std::uint16_t ICO_IMAGE_TYPE = 1;
	static constexpr std::uint16_t CUR_IMAGE_TYPE = 2;

	try
	{
		file.read(reinterpret_cast<char*>(&header_obj), sizeof(header_obj));
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error{ std::format("Failed to read {} bytes from ICO header!", sizeof(header_obj)) };
	}

	if (0 != header_obj.reserved)
	{
		throw std::invalid_argument{ std::format("Header reserved bytes are 0x{:X}, expecting 0x{:X}!", header_obj.reserved, 0) };
	}

	if (CUR_IMAGE_TYPE == header_obj.type)
	{
		throw std::invalid_argument{ "Image is of CUR type, not ICO!" };
	}

	if (ICO_IMAGE_TYPE != header_obj.type)
	{
		throw std::invalid_argument{ std::format("Image type 0x{:X} is invalid!", header_obj.type) };
	}

	if (0 == header_obj.entries_count)
	{
		throw std::invalid_argument{ "Icon does not have image entries!" };
	}
}

void ico_file::read_entries(std::ifstream& file)
{
	entries.resize(header_obj.entries_count);

	try
	{
		file.read(reinterpret_cast<char*>(entries.data()), entries.size() * sizeof(entry));
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error{ std::format("Failed to read {} bytes from ICO entry!", entries.size() * sizeof(entry)) };
	}
}

void ico_file::read_images(std::ifstream& file)
{
	std::vector<std::uint8_t> image = {};

	for (const entry& entry : entries)
	{
		if (0 != entry.reserved)
		{
			throw std::invalid_argument{ std::format("Entry's reserved byte is 0x{:X}, excepting 0x{:X}!", entry.reserved, 0) };
		}

		if (0 != entry.planes && 1 != entry.planes)
		{
			throw std::invalid_argument{ std::format("Entry's color planes is 0x{:X}, expecting 0x{:X} or 0x{:X}!", entry.planes, 0, 1) };
		}

		images.push_back(read_image(file, entry.image_size));
	}
}

} // namespace icon_changer
