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

#include <cstdint>
#include <print>

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

///
/// \brief CLI entry point for icon changing.
/// \details Handles `--version` argument, validates input, and initiates the
/// icon change.
/// \param argument_count: Number of arguments.
/// \param arguments: Argument values.
///
extern void change_icon_cli(std::int32_t argument_count, const char** arguments);

///
/// \brief Entry point to initiate the icon replacement in an executable.
/// \details Verifies files existence and forwards the call to the secure
/// version.
/// \param icon_path: The path to the `.ico` file.
/// \param executable_path: The path to the target `.exe` file.
///
extern void change_icon(std::string_view icon_path, std::string_view executable_path);

} // namespace icon_changer
