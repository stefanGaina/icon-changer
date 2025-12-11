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

#include "bmp_file.hpp"

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

bmp_file::bmp_file(const std::string_view file_path)
{
	std::ifstream file = open_file(file_path);

	read_header(file);
	read_image(file);
}

bmp_file::header bmp_file::get_header() const noexcept
{
	return header_obj;
}

std::vector<std::uint8_t>& bmp_file::get_image() noexcept
{
	return image;
}

void bmp_file::read_header(std::ifstream& file)
{
	try
	{
		file.read(reinterpret_cast<char*>(&header_obj), sizeof(header_obj));
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error{ std::format("Failed to read {} bytes from BMP header!", sizeof(header_obj)) };
	}

	LOG("type: {}", header_obj.type);
	LOG("file_size: {}", header_obj.file_size);
	LOG("reserved1: {}", header_obj.reserved1);
	LOG("reserved2: {}", header_obj.reserved2);
	LOG("image_offset: {}\n", header_obj.image_offset);
}

void bmp_file::read_image(std::ifstream& file)
{
	image.resize(header_obj.file_size - sizeof(header));

	try
	{
		file.read(reinterpret_cast<char*>(image.data()), image.size());
	}
	catch (const std::exception& exception)
	{
		throw std::runtime_error{ std::format("Failed to read {} bytes from BMP image!", image.size()) };
	}
}

} // namespace icon_changer
