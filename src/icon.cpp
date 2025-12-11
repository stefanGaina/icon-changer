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
#include <filesystem>
#include <format>

#include "bmp_file.hpp"

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

icon::icon(const std::string_view file_path)
    : header{}
    , images{}
{
	const std::string file_type = std::filesystem::path{ file_path }.extension().string();

	if (".ico" == file_type)
	{
		load_ico(file_path);
		return;
	}

	if (".bmp" == file_type)
	{
		load_bmp(file_path);
		return;
	}

	throw std::invalid_argument{ std::format("File type \"{}\" is not supported!", file_type) };
}

std::vector<std::uint8_t>& icon::get_header()
{
	return header;
}

std::vector<std::vector<std::uint8_t>>& icon::get_images() noexcept
{
	return images;
}

void icon::load_ico(const std::string_view file_path)
{
	ico_file                  ico_file = { file_path };
	std::vector<std::uint8_t> bytes    = {};
	std::uint16_t             id       = 0;

	header = serialize(ico_file.get_header());

	for (ico_file::entry entry : ico_file.get_entries())
	{
		assert(0 == entry.reserved);
		assert(0 == entry.planes || 1 == entry.planes);

		LOG("width: {}", entry.width);
		LOG("height: {}", entry.height);
		LOG("color_count: {}", entry.color_count);
		LOG("reserved: {}", entry.reserved);
		LOG("planes: {}", entry.planes);
		LOG("bit_count: {}", entry.bit_count);
		LOG("image_size: {}", entry.image_size);
		LOG("image_offset: {}", entry.image_offset);

		entry.image_offset = ++id;
		LOG("image_id: {}\n", *reinterpret_cast<std::uint16_t*>(&entry.image_offset));

		bytes = serialize(entry);
		header.insert(header.end(), bytes.begin(), bytes.end() - 2);
	}

	images = std::move(ico_file.get_images());
}

void icon::load_bmp(const std::string_view file_path)
{
	bmp_file                  bmp_file = { file_path };
	std::vector<std::uint8_t> bytes    = {};

	header = serialize(ico_file::header{ 0, 1, 1 });

	bytes = serialize(dib_header_to_entry(bmp_file.get_image()));
	header.insert(header.end(), bytes.begin(), bytes.end() - 2);

	images.push_back(std::move(bmp_file.get_image()));
}

ico_file::entry icon::dib_header_to_entry(std::vector<std::uint8_t>& dib_image)
{
	static constexpr std::size_t   HEIGHT_OFFSET = sizeof(std::uint32_t) + sizeof(std::int32_t);
	static constexpr std::uint32_t BI_RGB        = 0;
	static constexpr std::uint16_t DEFAULT_ID    = 1;

	ico_file::entry      entry      = {};
	bmp_file::dib_header dib_header = {};

	std::memcpy(&dib_header, dib_image.data(), sizeof(dib_header));

	LOG("header_size: {}", dib_header.header_size);
	LOG("width: {}", dib_header.width);
	LOG("height: {}", dib_header.height);
	LOG("planes: {}", dib_header.planes);
	LOG("bit_count: {}", dib_header.bit_count);
	LOG("compression_method: {}", dib_header.compression_method);
	LOG("image_size: {}", dib_header.image_size);
	LOG("horizontal_resolution: {}", dib_header.horizontal_resolution);
	LOG("vertical_resolution: {}", dib_header.vertical_resolution);
	LOG("color_count: {}", dib_header.color_count);
	LOG("ignored: {}\n", dib_header.ignored);

	if (sizeof(dib_header) != dib_header.header_size)
	{
		throw std::invalid_argument{ std::format("BMP header is not BITMAPINFOHEADER! (size: {})", sizeof(dib_header.header_size)) };
	}

	if (256 < dib_header.width)
	{
		throw std::invalid_argument{ std::format("Width {} is larger than the 256 limit!", dib_header.width) };
	}

	if (256 < dib_header.height)
	{
		throw std::invalid_argument{ std::format("Height {} is larger than the 256 limit!", dib_header.height) };
	}

	if (BI_RGB != dib_header.compression_method)
	{
		throw std::invalid_argument{ std::format("{} compression method is not supported!", dib_header.compression_method) };
	}

	entry.width        = static_cast<std::uint8_t>(dib_header.width);
	entry.height       = static_cast<std::uint8_t>(dib_header.height);
	entry.color_count  = static_cast<std::uint8_t>(dib_header.color_count);
	entry.reserved     = 0;
	entry.planes       = dib_header.planes;
	entry.bit_count    = dib_header.bit_count;
	entry.image_size   = static_cast<std::uint32_t>(dib_image.size());
	entry.image_offset = DEFAULT_ID;

	dib_header.height *= 2;
	std::memcpy(dib_image.data() + HEIGHT_OFFSET, &dib_header.height, sizeof(dib_header.height));

	return entry;
}

} // namespace icon_changer
