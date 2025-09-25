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

#include <print>
#include <cstdlib>
#include <cassert>

#include "icon_changer.hpp"
#include "ansi_color_codes.hpp"

////////////////////////////////////////////////////////////////////////////////
// ENTRY POINT
////////////////////////////////////////////////////////////////////////////////

std::int32_t main(const std::int32_t argument_count, const char** const arguments)
{
	using namespace icon_changer;

	assert(0 < argument_count);
	assert(nullptr != arguments);

	try
	{
		if (1 == argument_count)
		{
			change_icon_gui();
			return EXIT_SUCCESS;
		}

		change_icon_cli(argument_count, arguments);
	}
	catch (const std::exception& exception)
	{
		std::println(RED "{}" CRESET, exception.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* read 1st 22 bytes from ico file: https://en.wikipedia.org/wiki/ICO_(file_format) */
