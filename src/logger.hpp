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

#ifndef NDEBUG

#include <print>

#include "ansi_color_codes.hpp"

#endif // NDEBUG

////////////////////////////////////////////////////////////////////////////////
// MACROS
////////////////////////////////////////////////////////////////////////////////

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
