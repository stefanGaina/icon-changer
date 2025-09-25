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

#include <print>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

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
	static constexpr std::uint16_t ICO_IMAGE_TYPE = 1;
	static constexpr std::uint16_t CUR_IMAGE_TYPE = 2;

	file.read(reinterpret_cast<char*>(&resource_header), sizeof(resource_header));

	if (0 != resource_header.reserved)
	{
		throw std::invalid_argument{ std::format("Header reserved bytes are {}, expecting 0!", resource_header.reserved) };
	}

	if (CUR_IMAGE_TYPE == resource_header.type)
	{
		throw std::invalid_argument{ "Image is of CUR type, not ICO!" };
	}

	if (ICO_IMAGE_TYPE != resource_header.type)
	{
		throw std::invalid_argument{ std::format("Image type {} is invalid!", resource_header.type) };
	}
}

std::vector<icon::icon_entry> icon::read_icon_entries(std::ifstream& file)
{
	std::vector<icon_entry> entries = {};

	read_header(file);

	entries.resize(resource_header.entries_count);
	file.read(reinterpret_cast<char*>(entries.data()), entries.size() * sizeof(icon_entry));

	return entries;
}

void icon::read_images(std::ifstream& file, const std::vector<icon_entry>& entries)
{
	std::vector<std::uint8_t> image = {};

	for (const icon_entry& entry : entries)
	{
		if (0 != entry.reserved)
		{
			throw std::invalid_argument{ std::format("Entry's reserved byte is {}, excepting 0!", entry.reserved) };
		}

		if (0 != entry.planes && 1 != entry.planes)
		{
			throw std::invalid_argument{ std::format("Entry's color planes is {}, expecting 0 or 1!", entry.planes) };
		}

		image.resize(entry.image_size);
		file.read(reinterpret_cast<char*>(image.data()), image.size());

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

		resource_entries.push_back(std::move(entry));
	}
}

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

	for (const entry& entry : resource_entries)
	{
		entry_bytes = serialize(entry);
		serialized_header.insert(serialized_header.end(), entry_bytes.begin(), entry_bytes.end());
	}

	return serialized_header;
}

std::vector<std::vector<std::uint8_t>>& icon::get_images() noexcept
{
	return images;
}

} // namespace icon_changer
