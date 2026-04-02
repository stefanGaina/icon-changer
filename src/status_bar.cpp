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

#include "status_bar.hpp"

#include <string>

////////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

// TODO
static std::string status_text = "";

static bool is_status_error = false;

static bool is_status_visible = false;

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// TODO
static void refresh_window(const HWND window) noexcept;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

void hide_status_message(const HWND window) noexcept
{
	is_status_visible = false;
	refresh_window(window);
}

void show_status_message(const HWND             window,
                         const std::string_view text,
                         const bool             is_error) noexcept
{
	status_text       = text;
	is_status_error   = is_error;
	is_status_visible = true;

	refresh_window(window);
	SetTimer(window, 9999, 4000, nullptr);
}

void draw_status_message(const HDC device_context,
                         RECT      rectangle) noexcept
{
	HBRUSH brush = nullptr;
	HICON  icon  = nullptr;

	if (!is_status_visible)
	{
		return;
	}

	rectangle.top = rectangle.bottom - 40;
	brush         = CreateSolidBrush(RGB(45, 45, 45));

	FillRect(device_context, &rectangle, brush);
	DeleteObject(brush);

	icon = is_status_error ? LoadIcon(nullptr, IDI_ERROR) : LoadIcon(nullptr, IDI_INFORMATION);
	if (icon)
	{
		DrawIconEx(device_context, rectangle.left + 8, rectangle.bottom - 32, icon, 24, 24, 0, nullptr, DI_NORMAL);
	}

	SetBkMode(device_context, TRANSPARENT);
	SetTextColor(device_context, RGB(220, 220, 220));

	rectangle.left += 36;
	DrawTextA(device_context, status_text.c_str(), -1, &rectangle, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

static void refresh_window(const HWND window) noexcept
{
	RECT rectangle = {};

	GetClientRect(window, &rectangle);
	InvalidateRect(window, &rectangle, true);
	UpdateWindow(window);
}

} // namespace icon_changer
