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

#include "gui.hpp"

#include <dwmapi.h>

#include "handlers.hpp"
#include "utility.hpp"

////////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

// TODO
static WNDPROC button_ptr = nullptr;

static WNDPROC edit_ptr = nullptr;

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// TODO
static void create_window(std::string_view class_name,
                          const WNDCLASS&  window_class);

static void enable_dark_titlebar(HWND window);

static void get_messages() noexcept;

static std::int64_t window_procedure(HWND          window,
                                     std::uint32_t message,
                                     std::uint64_t param1,
                                     std::int64_t  param2) noexcept;

static std::int64_t button_procedure(HWND          window,
                                     std::uint32_t message,
                                     std::uint64_t param1,
                                     std::int64_t  param2) noexcept;

static std::int64_t edit_procedure(HWND          window,
                                   std::uint32_t message,
                                   std::uint64_t param1,
                                   std::int64_t  param2) noexcept;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

void change_icon_gui()
{
	const std::string_view class_name   = "icon-changer-window";
	WNDCLASS               window_class = {};

	ShowWindow(GetConsoleWindow(), SW_HIDE);

	window_class.lpfnWndProc   = window_procedure;
	window_class.hInstance     = GetModuleHandle(nullptr);
	window_class.lpszClassName = class_name.data();
	window_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

	RegisterClass(&window_class);
	create_window(class_name, window_class);

	get_messages();
}

static void create_window(const std::string_view class_name,
                          const WNDCLASS&        window_class)
{
	const std::int32_t window_width  = 400;
	const std::int32_t window_height = 300;
	const HWND         window        = CreateWindowEx(0,
                                       class_name.data(),
                                       std::format("icon-changer {}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH).c_str(),
                                       WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                       (GetSystemMetrics(SM_CXSCREEN) - window_width) / 2,
                                       (GetSystemMetrics(SM_CYSCREEN) - window_height) / 2,
                                       window_width,
                                       window_height,
                                       nullptr,
                                       nullptr,
                                       window_class.hInstance,
                                       nullptr);

	if (nullptr == window)
	{
		throw std::runtime_error{ std::format("Failed to create window! (error code: {})", GetLastError()) };
	}

	enable_dark_titlebar(window);
	ShowWindow(window, SW_SHOW);
}

static void enable_dark_titlebar(const HWND window)
{
	std::int32_t       value      = true;
	const std::int64_t error_code = DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

	if (S_OK != error_code)
	{
		throw std::runtime_error{ std::format("Failed to set dark mode! (error code: {})", error_code) };
	}
}

static void get_messages() noexcept
{
	MSG message = {};

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

static std::int64_t window_procedure(const HWND          window,
                                     const std::uint32_t message,
                                     const std::uint64_t param1,
                                     const std::int64_t  param2) noexcept
{
	switch (message)
	{
		case WM_CREATE:
		{
			return handle_create(window, button_procedure, edit_procedure, button_ptr, edit_ptr);
		}
		case WM_COMMAND:
		{
			return handle_command(window, param1);
		}
		case WM_ERASEBKGND:
		{
			return handle_erase_background(window, param1);
		}
		case WM_DRAWITEM:
		{
			if (std::optional<std::int64_t> result = handle_draw_item(window, param2))
			{
				return *result;
			}
			break;
		}
		case WM_CTLCOLORSTATIC:
			[[fallthrough]];
		case WM_CTLCOLOREDIT:
		{
			return handle_color_static(param1);
		}
		case WM_TIMER:
		{
			handle_timer(window, param1);
			break;
		}
		case WM_DESTROY:
		{
			return handle_destroy();
		}
	}

	return DefWindowProc(window, message, param1, param2);
}

static std::int64_t button_procedure(const HWND          window,
                                     const std::uint32_t message,
                                     const std::uint64_t param1,
                                     const std::int64_t  param2) noexcept
{
	switch (message)
	{
		case WM_MOUSEMOVE:
		{
			handle_mouse_move(window);
			break;
		}
		case WM_MOUSELEAVE:
		{
			handle_mouse_leave(window);
			break;
		}
	}

	return CallWindowProc(button_ptr, window, message, param1, param2);
}

static std::int64_t edit_procedure(const HWND          window,
                                   const std::uint32_t message,
                                   const std::uint64_t param1,
                                   const std::int64_t  param2) noexcept
{
	switch (message)
	{
		case WM_PAINT:
		{
			handle_paint(window);
			break;
		}
		case WM_DROPFILES:
		{
			handle_drop_files(window, param1);
			break;
		}
	}

	return CallWindowProc(edit_ptr, window, message, param1, param2);
}

} // namespace icon_changer
