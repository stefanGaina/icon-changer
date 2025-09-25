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
#include <fstream>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////////////////////

///
/// \brief
///
#define PACKED __attribute__((packed))

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

class icon final
{
public:
	struct PACKED header final
	{
		std::uint16_t reserved;
		std::uint16_t type;
		std::uint16_t entries_count;
	};

	struct PACKED entry final
	{
		std::uint8_t width;
		std::uint8_t height;
		std::uint8_t color_count;
		std::uint8_t reserved;
		std::uint16_t planes;
		std::uint16_t bit_count;
		std::uint32_t resource_size;
		std::uint16_t icon_id;
	};

public:
	icon(std::string_view file_path);

	std::vector<std::uint8_t> get_header() const;

	std::vector<entry>& get_entries() noexcept;

	std::vector<std::vector<std::uint8_t>>& get_images() noexcept;

private:
	struct PACKED icon_entry final
	{
		std::uint8_t  width;
		std::uint8_t  height;
		std::uint8_t  color_count;
		std::uint8_t  reserved;
		std::uint16_t planes;
		std::uint16_t bit_count;
		std::uint32_t image_size;
		std::uint32_t image_offset;
	};

private:
	static std::ifstream open_file(std::string_view file_path);

	static std::vector<std::uint8_t> serialize(const header& header);

	static std::vector<std::uint8_t> serialize(const entry& entry);

	void read_header(std::ifstream& file);

	[[nodiscard]] std::vector<icon_entry> read_icon_entries(std::ifstream& file);

	void read_images(std::ifstream& file, const std::vector<icon_entry>& entries);

	void convert_entries(const std::vector<icon_entry>& entries);

private:
	header resource_header;

	std::vector<entry> resource_entries;

	std::vector<std::vector<std::uint8_t>> images;
};

} // namespace icon_changer
