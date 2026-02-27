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

#include <fstream>
#include <print>
#include <string_view>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Macro to align structs to prevent padding.
/// \details This ensures that the struct's memory layout matches the layout of
/// packed binary data, such as when reading icon files in a specific format.
///
#define PACKED __attribute__((packed))

// Regular text
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define CYN "\e[0;36m"

// Reset
#define CRESET "\e[0m"

#ifndef NDEBUG

///
/// \brief Logs a debug message to the terminal in cyan color.
/// \details Message are prefixed with "[DEBUG]". It's available only in
/// debug builds.
/// \param format: The format string (compatible with std::print).
/// \param __VA_ARGS__: Variadic arguments to be formatted into the message.
///
#define LOG(format, ...) std::println(CYN "[DEBUG] " format CRESET, ##__VA_ARGS__)

#else

///
/// \brief Logs are stripped from compilation.
///
#define LOG(format, ...)

#endif // NDEBUG

////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief The major version of the application.
///
inline constexpr std::uint16_t VERSION_MAJOR = 1;

///
/// \brief The minor version of the application.
///
inline constexpr std::uint16_t VERSION_MINOR = 1;

///
/// \brief The patch version of the application.
///
inline constexpr std::uint16_t VERSION_PATCH = 0;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////

///
/// \brief Opens the specified file in binary mode and sets exceptions for
/// failbit and badbit.
/// \param file_path: The path to the file to be opened.
/// \returns An input file stream for reading the file.
///
extern std::ifstream open_file(std::string_view file_path);

///
/// \brief Serializes the header into a byte vector.
/// \param header: The header structure to be serialized.
/// \returns A vector of bytes representing the serialized header.
///
template <typename T> std::vector<std::uint8_t> serialize(const T& obj);

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

template <typename T> std::vector<std::uint8_t> serialize(const T& obj)
{
	std::vector<std::uint8_t> bytes = {};

	bytes.resize(sizeof(obj));
	std::memcpy(bytes.data(), &obj, sizeof(obj));

	return bytes;
}

} // namespace icon_changer
