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

#include "bitmap.hpp"

#include <fstream>
#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////
// STRUCT DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct BitmapFileHeader
{
	std::uint16_t bfType;
	std::uint32_t bfSize;
	std::uint16_t bfReserved1;
	std::uint16_t bfReserved2;
	std::uint32_t bfOffBits;
};

struct BitmapInfoHeader
{
	std::uint32_t biSize;
	std::int32_t  biWidth;
	std::int32_t  biHeight;
	std::uint16_t biPlanes;
	std::uint16_t biBitCount;
	std::uint32_t biCompression;
	std::uint32_t biSizeImage;
	std::int32_t  biXPelsPerMeter;
	std::int32_t  biYPelsPerMeter;
	std::uint32_t biClrUsed;
	std::uint32_t biClrImportant;
};
#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
// METHOD DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

	bool bitmap::loadFromImage(const std::string& path)
	{
		std::ifstream file{ path, std::ios::binary };
		if (!file.is_open())
		{
			throw std::invalid_argument{ "Failed to open BMP file: " + path };
		}

		BitmapFileHeader file_header{};
		BitmapInfoHeader info_header{};

		file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
		file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

		if (file_header.bfType != 0x4D42) // 'BM'
		{
			throw std::runtime_error{ "Not a valid BMP file: " + path };
		}
		if (info_header.biCompression != 0) {
			throw std::runtime_error("Unsupported BMP compression: " + path);
		}

		width = info_header.biWidth;
		height = info_header.biHeight;
		bitDepth = info_header.biBitCount;

		const bool topDown = (info_header.biHeight < 0);
		const std::size_t bytesPerPixel = bitDepth / 8;
		const std::size_t paddedRowSize = ((bitDepth * width + 31) / 32) * 4;
		const std::size_t rawRowSize = width * bytesPerPixel;

		pixels.resize(width * height * bytesPerPixel);

		file.seekg(file_header.bfOffBits, std::ios::beg);

		std::vector<std::uint8_t> row(paddedRowSize);

		for (int y = 0; y < height; ++y) {
			file.read(reinterpret_cast<char*>(row.data()), paddedRowSize);

			int destY = topDown ? y : (height - 1 - y);

			std::copy_n(row.data(),
				rawRowSize,
				&pixels[destY * rawRowSize]
			);
		}

		return true;
	}

	bool bitmap::saveToIco(const std::string& path) const {
		if (width <= 0 || height <= 0 || bitDepth != 24) {
			throw std::runtime_error("Bitmap must be valid and 24-bit to save as ICO.");
		}

		std::ofstream out(path, std::ios::binary);
		if (!out.is_open()) {
			throw std::runtime_error("Failed to write ICO file: " + path);
		}

		const std::size_t imageSize = width * height * 4;
		const std::size_t maskSize = ((width + 31) / 32) * 4 * height;
		const std::size_t dataSize = imageSize + maskSize;

		struct ICONDIR {
			std::uint16_t idReserved = 0;
			std::uint16_t idType = 1; // Icon
			std::uint16_t idCount = 1; // One image
		} iconDir;
		out.write(reinterpret_cast<const char*> (&iconDir), sizeof(iconDir));

		struct ICONDIRENTRY {
			std::uint8_t  bWidth;
			std::uint8_t  bHeight;
			std::uint8_t  bColorCount = 0;
			std::uint8_t  bReserved = 0;
			std::uint16_t wPlanes = 1;
			std::uint16_t wBitCount = 32;
			std::uint32_t dwBytesInRes;
			std::uint32_t dwImageOffset;
		} entry;
		entry.bWidth = static_cast<std::uint8_t> (width);
		entry.bHeight = static_cast<std::uint8_t> (height);
		entry.dwBytesInRes = sizeof(BitmapInfoHeader) + dataSize;
		entry.dwImageOffset = sizeof(ICONDIR) + sizeof(ICONDIRENTRY);

		out.write(reinterpret_cast<const char*> (&entry), sizeof(entry));
		BitmapInfoHeader bih{};
		bih.biSize = sizeof(BitmapInfoHeader);
		bih.biWidth = width;
		bih.biHeight = height * 2;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = 0;
		bih.biSizeImage = imageSize;

		out.write(reinterpret_cast<const char*>(&bih), sizeof(bih));

		for (int y = height - 1; y >= 0; --y) {
			for (int x = 0; x < width; ++x) {
				int i = (y * width + x) * 3;
				std::uint8_t b = pixels[i + 0];
				std::uint8_t g = pixels[i + 1];
				std::uint8_t r = pixels[i + 2];
				std::uint8_t a = 255;

				out.put(b);
				out.put(g);
				out.put(r);
				out.put(a);
			}
		}
		std::vector<std::uint8_t> mask(maskSize, 0x00);
		out.write(reinterpret_cast<const char*> (mask.data()), mask.size());

		return true;
	}
} // namespace icon_changer