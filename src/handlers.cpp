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

#include "handlers.hpp"

#include "icon_changer.hpp"
#include "status_bar.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

namespace icon_changer
{

// TODO
constexpr std::uint64_t BACKGROUND_COLOR = RGB(30, 30, 30);
constexpr std::uint64_t BTN_COLOR        = RGB(60, 60, 60);
constexpr std::uint64_t BTN_HOVER        = RGB(80, 80, 80);
constexpr std::uint64_t BTN_PRESS        = RGB(40, 40, 40);
constexpr std::uint64_t TEXT_COLOR       = RGB(220, 220, 220);
constexpr std::uint64_t EDIT_BG          = RGB(50, 50, 50);

////////////////////////////////////////////////////////////////////////////////
// LOCAL VARIABLES
////////////////////////////////////////////////////////////////////////////////

// TODO
static HWND change_icon_button = nullptr;
static HWND edit_executable    = nullptr;
static HWND edit_icon          = nullptr;

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

// TODO
static void handle_change_icon(HWND window) noexcept;

static void draw_buttons(LPDRAWITEMSTRUCT draw_item) noexcept;

static void draw_boxes(LPDRAWITEMSTRUCT draw_item) noexcept;

static void update_button_state() noexcept;

static void browse_file(HWND             owner,
                        HWND             edit,
                        std::string_view filter) noexcept;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

std::int64_t handle_create(const HWND  window,
                           void* const button_procedure,
                           void* const edit_procedure,
                           WNDPROC&    button_ptr,
                           WNDPROC&    edit_ptr) noexcept
{
	const HWND browse_executable_button = CreateWindow("BUTTON",
	                                                   "...",
	                                                   WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
	                                                   320,
	                                                   50,
	                                                   30,
	                                                   30,
	                                                   window,
	                                                   reinterpret_cast<HMENU>(201),
	                                                   reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE)),
	                                                   nullptr);
	const HWND browse_icon_button       = CreateWindow("BUTTON",
                                                 "...",
                                                 WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                                 320,
                                                 100,
                                                 30,
                                                 30,
                                                 window,
                                                 reinterpret_cast<HMENU>(202),
                                                 reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE)),
                                                 nullptr);

	SetWindowLongPtr(browse_executable_button, GWLP_USERDATA, 0);
	SetWindowLongPtr(browse_executable_button, GWLP_WNDPROC, reinterpret_cast<std::int64_t>(button_procedure));

	SetWindowLongPtr(browse_icon_button, GWLP_USERDATA, 0);
	SetWindowLongPtr(browse_icon_button, GWLP_WNDPROC, reinterpret_cast<std::int64_t>(button_procedure));

	edit_executable = CreateWindow("STATIC",
	                               "",
	                               WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
	                               50,
	                               50,
	                               260,
	                               30,
	                               window,
	                               reinterpret_cast<HMENU>(101),
	                               reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE)),
	                               nullptr);

	SetWindowLongPtr(edit_executable, GWLP_USERDATA, 0);
	DragAcceptFiles(edit_executable, true);
	edit_ptr = reinterpret_cast<WNDPROC>(SetWindowLongPtr(edit_executable, GWLP_WNDPROC, reinterpret_cast<std::int64_t>(edit_procedure)));

	edit_icon = CreateWindow("STATIC",
	                         "",
	                         WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
	                         50,
	                         100,
	                         260,
	                         30,
	                         window,
	                         reinterpret_cast<HMENU>(102),
	                         reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE)),
	                         nullptr);

	SetWindowLongPtr(edit_icon, GWLP_USERDATA, 0);
	DragAcceptFiles(edit_icon, true);
	SetWindowLongPtr(edit_icon, GWLP_WNDPROC, reinterpret_cast<std::int64_t>(edit_procedure));

	change_icon_button = CreateWindow("BUTTON",
	                                  "Change Icon",
	                                  WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_DISABLED,
	                                  120,
	                                  160,
	                                  160,
	                                  40,
	                                  window,
	                                  reinterpret_cast<HMENU>(1),
	                                  reinterpret_cast<HINSTANCE>(GetWindowLongPtr(window, GWLP_HINSTANCE)),
	                                  nullptr);

	SetWindowLongPtr(change_icon_button, GWLP_USERDATA, 0);
	button_ptr = reinterpret_cast<WNDPROC>(SetWindowLongPtr(change_icon_button, GWLP_WNDPROC, reinterpret_cast<std::int64_t>(button_procedure)));

	return 0;
}

std::int64_t handle_command(const HWND          window,
                            const std::uint64_t param1) noexcept
{
	switch (LOWORD(param1))
	{
		case 1:
		{
			handle_change_icon(window);
			break;
		}
		case 201:
		{
			browse_file(window, edit_executable, "Executable Files\0*.exe\0All Files\0*.*\0");
			break;
		}
		case 202:
		{
			browse_file(window, edit_icon, "Icon/BMP Files\0*.ico;*.bmp\0All Files\0*.*\0");
			break;
		}
	}

	update_button_state();
	return 0;
}

std::int64_t handle_erase_background(const HWND          window,
                                     const std::uint64_t param1) noexcept
{
	const HDC    device_context = reinterpret_cast<HDC>(param1);
	const HBRUSH brush          = CreateSolidBrush(BACKGROUND_COLOR);
	RECT         rectangle      = {};

	GetClientRect(window, &rectangle);
	FillRect(device_context, &rectangle, brush);
	DeleteObject(brush);

	draw_status_message(device_context, rectangle);
	return 1;
}

std::optional<std::int64_t> handle_draw_item(const HWND          window,
                                             const std::uint64_t param2) noexcept
{
	const LPDRAWITEMSTRUCT draw_item = reinterpret_cast<LPDRAWITEMSTRUCT>(param2);

	if (1 == draw_item->CtlID || 201 == draw_item->CtlID || 202 == draw_item->CtlID)
	{
		draw_buttons(draw_item);
		return 1;
	}

	if (101 == draw_item->CtlID || 102 == draw_item->CtlID)
	{
		draw_boxes(draw_item);
		return 1;
	}

	return std::nullopt;
}

std::int64_t handle_color_static(const std::uint64_t param1) noexcept
{
	const HDC device_context = reinterpret_cast<HDC>(param1);

	update_button_state();

	SetBkMode(device_context, TRANSPARENT);
	SetTextColor(device_context, TEXT_COLOR);
	return reinterpret_cast<std::int64_t>(GetStockObject(NULL_BRUSH));
}

void handle_timer(const HWND          window,
                  const std::uint64_t param1) noexcept
{
	if (9999 != param1)
	{
		return;
	}

	hide_status_message(window);
}

std::int64_t handle_destroy() noexcept
{
	PostQuitMessage(0);
	return 0;
}

void handle_mouse_move(const HWND window) noexcept
{
	TRACKMOUSEEVENT track_mouse_event = {};

	if (GetWindowLongPtr(window, GWLP_USERDATA))
	{
		return;
	}

	SetWindowLongPtr(window, GWLP_USERDATA, 1);

	track_mouse_event.cbSize    = sizeof(track_mouse_event);
	track_mouse_event.dwFlags   = TME_LEAVE;
	track_mouse_event.hwndTrack = window;

	TrackMouseEvent(&track_mouse_event);
	InvalidateRect(window, nullptr, true);
}

void handle_mouse_leave(const HWND window) noexcept
{
	SetWindowLongPtr(window, GWLP_USERDATA, 0);
	InvalidateRect(window, nullptr, true);
}

void handle_paint(const HWND window) noexcept
{
	PAINTSTRUCT  paint          = {};
	const HDC    device_context = BeginPaint(window, &paint);
	const HBRUSH brush          = CreateSolidBrush(EDIT_BG);
	RECT         rectangle      = {};
	char         text[512]      = {};

	GetClientRect(window, &rectangle);
	FillRect(device_context, &rectangle, brush);
	DeleteObject(brush);

	SetBkMode(device_context, TRANSPARENT);
	SetTextColor(device_context, TEXT_COLOR);

	GetWindowTextA(window, text, sizeof(text));
	rectangle.left += 6;

	if (0 != std::strlen(text))
	{
		DrawTextA(device_context, text, -1, &rectangle, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

		EndPaint(window, &paint);
		return;
	}

	SetTextColor(device_context, RGB(150, 150, 150));
	DrawTextA(device_context,
	          window == edit_executable ? "Choose executable or drag it here..." : "Choose icon or drag it here...",
	          -1,
	          &rectangle,
	          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	EndPaint(window, &paint);
}

void handle_drop_files(const HWND          window,
                       const std::uint64_t param1) noexcept
{
	const HDROP drop_file = reinterpret_cast<HDROP>(param1);
	char        path[512] = "";

	DragQueryFileA(drop_file, 0, path, sizeof(path));
	DragFinish(drop_file);

	SetWindowTextA(window, path);
	update_button_state();
}

static void handle_change_icon(const HWND window) noexcept
{
	char executable_path[512] = "";
	char icon_path[512]       = "";

	GetWindowTextA(edit_executable, executable_path, sizeof(executable_path));
	GetWindowTextA(edit_icon, icon_path, sizeof(icon_path));

	try
	{
		change_icon(icon_path, executable_path);
		show_status_message(window, "Icon changed successfully!");
	}
	catch (const std::exception& exception)
	{
		show_status_message(window, exception.what(), true);
	}
}

static void draw_buttons(const LPDRAWITEMSTRUCT draw_item) noexcept
{
	const COLORREF button_color     = draw_item->itemState & ODS_SELECTED                    ? BTN_PRESS
	                                  : !IsWindowEnabled(draw_item->hwndItem)                ? RGB(120, 120, 120)
	                                  : GetWindowLongPtr(draw_item->hwndItem, GWLP_USERDATA) ? BTN_HOVER
	                                                                                         : BTN_COLOR;
	const HBRUSH   background_brush = CreateSolidBrush(BACKGROUND_COLOR);
	const HBRUSH   brush            = CreateSolidBrush(button_color);
	const HPEN     pen              = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
	const HBRUSH   previous_brush   = reinterpret_cast<HBRUSH>(SelectObject(draw_item->hDC, brush));
	const HPEN     previous_pen     = reinterpret_cast<HPEN>(SelectObject(draw_item->hDC, pen));
	char           text[128]        = "";

	FillRect(draw_item->hDC, &draw_item->rcItem, background_brush);
	DeleteObject(background_brush);

	RoundRect(draw_item->hDC, draw_item->rcItem.left, draw_item->rcItem.top, draw_item->rcItem.right, draw_item->rcItem.bottom, 12, 12);
	SetBkMode(draw_item->hDC, TRANSPARENT);
	SetTextColor(draw_item->hDC, TEXT_COLOR);

	GetWindowTextA(draw_item->hwndItem, text, sizeof(text));
	DrawTextA(draw_item->hDC, text, -1, &draw_item->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	SelectObject(draw_item->hDC, previous_brush);
	SelectObject(draw_item->hDC, previous_pen);
	DeleteObject(brush);
	DeleteObject(pen);
}

static void draw_boxes(const LPDRAWITEMSTRUCT draw_item) noexcept
{
	const HBRUSH brush     = CreateSolidBrush(EDIT_BG);
	char         text[512] = "";

	FillRect(draw_item->hDC, &draw_item->rcItem, brush);
	DeleteObject(brush);

	SetBkMode(draw_item->hDC, TRANSPARENT);
	SetTextColor(draw_item->hDC, TEXT_COLOR);

	draw_item->rcItem.left += 6;

	GetWindowTextA(draw_item->hwndItem, text, sizeof(text));
	DrawTextA(draw_item->hDC, text, -1, &draw_item->rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

static void update_button_state() noexcept
{
	char buffer1[512] = "";
	char buffer2[512] = "";

	GetWindowTextA(edit_executable, buffer1, sizeof(buffer1));
	GetWindowTextA(edit_icon, buffer2, sizeof(buffer2));

	EnableWindow(change_icon_button, 0 != std::strlen(buffer1) && 0 != std::strlen(buffer2));
	InvalidateRect(change_icon_button, nullptr, true);
}

static void browse_file(const HWND             owner,
                        const HWND             edit,
                        const std::string_view filter) noexcept
{
	char         path[512] = "";
	OPENFILENAME ofn       = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = owner;
	ofn.lpstrFile   = path;
	ofn.nMaxFile    = sizeof(path);
	ofn.lpstrFilter = filter.data();
	ofn.Flags       = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	if (!GetOpenFileName(&ofn))
	{
		return;
	}

	SetWindowTextA(edit, path);
	update_button_state();
}

} // namespace icon_changer
