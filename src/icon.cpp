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

#include "icon.hpp"

#include <cassert>
#include <format>
#include <filesystem> //for std::remove

#include "logger.hpp"
#include "bitmap.hpp"

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

icon::icon(const std::string_view file_path)
    : resource_header{}
    , resource_entries{}
    , images{}
{
	std::ifstream                 file    = open_file(file_path);
	const std::vector<icon_entry> entries = read_icon_entries(file);

	read_images(file, entries);
	convert_entries(entries);
}

std::vector<std::uint8_t> icon::get_header() const
{
	std::vector<std::uint8_t> serialized_header = {};
	std::vector<std::uint8_t> header_bytes      = serialize(resource_header);
	std::vector<std::uint8_t> entry_bytes       = {};

	serialized_header.insert(serialized_header.end(), header_bytes.begin(), header_bytes.end());

	for (const entry& e : resource_entries)
	{
		entry_bytes = serialize(e);
		serialized_header.insert(serialized_header.end(), entry_bytes.begin(), entry_bytes.end());
	}

	return serialized_header;
}

std::vector<std::vector<std::uint8_t>>& icon::get_images() noexcept
{
	return images;
}

std::ifstream icon::open_file(const std::string_view file_path)
{
	std::ifstream file = std::ifstream{ file_path.data(), std::ios::binary };

	if (!file.is_open())
	{
		throw std::invalid_argument{ std::format("Failed to open \"{}\"!", file_path) };
	}

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	return std::move(file);
}

std::vector<std::uint8_t> icon::serialize(const icon::header& header)
{
	std::vector<std::uint8_t> bytes = {};

	bytes.resize(sizeof(header));
	std::memcpy(bytes.data(), &header, sizeof(header));

	return bytes;
}

std::vector<std::uint8_t> icon::serialize(const icon::entry& entry)
{
	std::vector<std::uint8_t> bytes = {};

	bytes.resize(sizeof(entry));
	std::memcpy(bytes.data(), &entry, sizeof(entry));

	return bytes;
}

void icon::read_header(std::ifstream& file)
{
	static constexpr std::uint16_t ICO_IMAGE_TYPE = 0x0001;
	static constexpr std::uint16_t CUR_IMAGE_TYPE = 0x0002;

	try
	{
		file.read(reinterpret_cast<char*>(&resource_header), sizeof(resource_header));
	}
	catch (const std::ios_base::failure& e)
	{
		throw std::runtime_error{ "Failed to read icon header from file." };
	}

	if (0x0000 != resource_header.reserved)
	{
		throw std::invalid_argument{ std::format("Header reserved bytes are 0x{:X}, expecting 0x{:X}!", resource_header.reserved, 0x0000) };
	}

	if (CUR_IMAGE_TYPE == resource_header.type)
	{
		throw std::invalid_argument{ "Image is of CUR type, not ICO!" };
	}

	if (ICO_IMAGE_TYPE != resource_header.type)
	{
		throw std::invalid_argument{ std::format("Image type 0x{:X} is invalid!", resource_header.type) };
	}

	if (0x0000 == resource_header.entries_count)
	{
		throw std::invalid_argument{ "Icon does not have image entries!" };
	}
}

std::vector<icon::icon_entry> icon::read_icon_entries(std::ifstream& file)
{
	std::vector<icon_entry> entries = {};

	read_header(file);

	entries.resize(resource_header.entries_count);

	try
	{
		file.read(reinterpret_cast<char*>(entries.data()), entries.size() * sizeof(icon_entry));
	}
	catch (const std::ios_base::failure& e)
	{
		throw std::runtime_error{ "Failed to read icon entry data from file." };
	}

	return entries;
}

void icon::read_images(std::ifstream&                 file,
                       const std::vector<icon_entry>& entries)
{
	std::vector<std::uint8_t> image = {};

	for (const icon_entry& entry : entries)
	{
		if (0x00 != entry.reserved)
		{
			throw std::invalid_argument{ std::format("Entry's reserved byte is 0x{:X}, excepting 0x{:X}!", entry.reserved, 0x00) };
		}

		if (0x0000 != entry.planes && 0x0001 != entry.planes)
		{
			throw std::invalid_argument{ std::format("Entry's color planes is 0x{:X}, expecting 0x{:X} or 0x{:X}!", entry.planes, 0x0000, 0x0001) };
		}

		image.resize(entry.image_size);

		try
		{
			file.read(reinterpret_cast<char*>(image.data()), image.size());
		}
		catch (const std::ios_base::failure& e)
		{
			throw std::runtime_error{ "Failed to read icon image data from file." };
		}

		images.push_back(std::move(image));
	}
}

void icon::convert_entries(const std::vector<icon_entry>& entries)
{
	entry         entry   = {};
	std::uint16_t icon_id = 0;

	for (const icon_entry& icon_entry : entries)
	{
		assert(0 == icon_entry.reserved);
		assert(0 == icon_entry.planes || 1 == icon_entry.planes);

		entry.width         = icon_entry.width;
		entry.height        = icon_entry.height;
		entry.color_count   = icon_entry.color_count;
		entry.reserved      = icon_entry.reserved;
		entry.planes        = icon_entry.planes;
		entry.bit_count     = icon_entry.bit_count;
		entry.resource_size = icon_entry.image_size;
		entry.icon_id       = ++icon_id;

		LOG("width: {}", entry.width);
		LOG("height: {}", entry.height);
		LOG("color_count: {}", entry.color_count);
		LOG("reserved: {}", entry.reserved);
		LOG("planes: {}", entry.planes);
		LOG("bit_count: {}", entry.bit_count);
		LOG("resource_size: {}", entry.resource_size);
		LOG("icon_id: {}\n", entry.icon_id);

		resource_entries.push_back(std::move(entry));
	}
}
icon icon::from_bmp(const std::string_view bmp_path) 
{
	bitmap bmp;
	if (!bmp.loadFromImage(bmp_path.data())) {
		throw std::runtime_error("Failed to load BMP image from: " + std::string(bmp_path));
	}

	const std::string temp_ico_path = std::string(bmp_path) + ".temp.ico";
	try {
		if (!bmp.saveToIco(temp_ico_path)) {
			throw std::runtime_error("Failed to save temporary .ICO file");
		}

		// Create icon from temporary ICO file
		icon ico{ temp_ico_path };

		// Clean up temporary file
		std::filesystem::remove(temp_ico_path);

		LOG("Successfully created icon from BMP: {}", bmp_path);
		return ico;
	}
	catch (...) {
		std::filesystem::remove(temp_ico_path);
		throw;
	}
}

} // namespace icon_changer
