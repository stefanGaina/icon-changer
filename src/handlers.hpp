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

#include <windows.h>

#include <optional>

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

// TODO
std::int64_t handle_create(HWND     window,
                           void*    button_procedure,
                           void*    edit_procedure,
                           WNDPROC& button_ptr,
                           WNDPROC& edit_ptr) noexcept;

std::int64_t handle_command(HWND          window,
                            std::uint64_t param1) noexcept;

std::int64_t handle_erase_background(HWND          window,
                                     std::uint64_t param1) noexcept;

std::optional<std::int64_t> handle_draw_item(HWND          window,
                                             std::uint64_t param2) noexcept;

std::int64_t handle_color_static(std::uint64_t param1) noexcept;

void handle_timer(HWND          window,
                  std::uint64_t param1) noexcept;

std::int64_t handle_destroy() noexcept;

void handle_mouse_move(HWND window) noexcept;

void handle_mouse_leave(HWND window) noexcept;

void handle_paint(HWND window) noexcept;

void handle_drop_files(HWND          window,
                       std::uint64_t param1) noexcept;

} // namespace icon_changer
