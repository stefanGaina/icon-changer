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

#include "cli.hpp"

#include <cassert>
#include <stdexcept>

#include "icon_changer.hpp"
#include "utility.hpp"

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief Prints information to the user about the usage of the icon changer.
///
static void print_help();

///
/// \brief Validates the number of command-line arguments.
/// \details If the argument count is incorrect, help is printed and an exception
/// is thrown for too few arguments.
/// \param argument_count: The number of command-line arguments passed.
///
static void validate_argument_count(std::int32_t argument_count);

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

void change_icon_cli(const std::int32_t argument_count,
                     const char** const arguments)
{
	assert(0 < argument_count);
	assert(nullptr != arguments);

	if (2 <= argument_count && ("--help" == std::string_view{ arguments[1] } || "-h" == std::string_view{ arguments[1] }))
	{
		print_help();
		return;
	}

	if (2 <= argument_count && ("--version" == std::string_view{ arguments[1] } || "-v" == std::string_view{ arguments[1] }))
	{
		std::println("icon-changer version {}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		return;
	}

	validate_argument_count(argument_count);
	change_icon(arguments[1], arguments[2]);
	std::println(GRN "Icon changed successfully!" CRESET);
}

static void print_help()
{
	std::println("Usage: icon-changer <path_to_icon> <path_to_exe>");
	std::println("valid icon formats are: ICO (recommended), BMP");
	std::println("valid program format is: EXE");
}

static void validate_argument_count(const std::int32_t argument_count)
{
	static constexpr std::size_t REQUIRED_ARGUMENT_COUNT = 3;

	if (REQUIRED_ARGUMENT_COUNT == argument_count)
	{
		return;
	}

	print_help();

	if (REQUIRED_ARGUMENT_COUNT > argument_count)
	{
		throw std::runtime_error{ std::format("{} parameter(s) missing!", REQUIRED_ARGUMENT_COUNT - argument_count) };
	}

	std::println(YEL "{} parameter(s) will be ignored..." CRESET, argument_count - REQUIRED_ARGUMENT_COUNT);
}

} // namespace icon_changer
