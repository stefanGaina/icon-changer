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

#include <gmock/gmock.h>

#include "icon.hpp"

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

class icon_mock final
{
public:
	MOCK_METHOD(std::vector<std::uint8_t>, get_header, (), (const));
	MOCK_METHOD(std::vector<std::vector<std::uint8_t>>&, get_images, (), ());

	icon_mock()
	{
		if (nullptr == obj)
		{
			return;
		}

		obj = std::make_unique<icon_mock>();
	}

	static std::unique_ptr<icon_mock> obj;
};

std::unique_ptr<icon_mock> icon_mock::obj = nullptr;

icon::icon(const std::string_view file_path)
{
}

std::vector<std::uint8_t> icon::get_header() const
{
	return icon_mock::obj->get_header();
}

std::vector<std::vector<std::uint8_t>>& icon::get_images() noexcept
{
	return icon_mock::obj->get_images();
}

} // namespace icon_changer
