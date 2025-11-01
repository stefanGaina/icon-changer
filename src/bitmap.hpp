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
#include <string>
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
////////////////////////////////////////////////////////////////////////////////
namespace icon_changer 
{

class bitmap
{
private:
	int                     width      = 0;
	int                     height     = 0;
	int                     bitDepth   = 0;
	std::vector<std::uint8_t> pixels; // BGR format

public:
	////////////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	////////////////////////////////////////////////////////////////////////////////

	[[nodiscard]] int getWidth() const noexcept { return width; }
	[[nodiscard]] int getHeight() const noexcept { return height; }
	[[nodiscard]] const std::vector<std::uint8_t>& getPixels() const noexcept { return pixels; }
	[[nodiscard]] int getBitDepth() const noexcept { return bitDepth; }

	bool loadFromImage(const std::string& path);
	bool saveToIco(const std::string& path) const;
};
} // namespace icon_changer