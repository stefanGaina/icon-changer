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

/* NOTE: WIN32_LEAN_AND_MEAN defined before including <windows.h>.
   This excludes rarely used APIs from windows.h and reduces build time. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <string>
#include "icon_changer.hpp"
#include "GUI_private.hpp"

////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////////////

static char exePath[MAX_PATH] = "";
static char icoPath[MAX_PATH] = "";

////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static HWND CreateLabel(const HWND parent, const char* text, const int x, const int y) 
{
    return CreateWindowA(GUI::Text::STATIC_CLASS, text, WS_CHILD | WS_VISIBLE,
        x, y, GUI::Layout::LABEL_WIDTH, GUI::Layout::LABEL_HEIGHT, 
        parent, NULL, NULL, NULL);
}

static HWND CreateButton(const HWND parent, const char* text, const int x, const int y, 
    const int width, const int height, const HMENU id) 
{
    return CreateWindowA(GUI::Text::BUTTON_CLASS, text, WS_CHILD | WS_VISIBLE,
        x, y, width, height, parent, id, NULL, NULL);
}

static void HandlePathSelection(const HWND hwnd, char* path, const char* filter) 
{
    OPENFILENAMEA ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    GetOpenFileNameA(&ofn);
}

static void HandleChangeIcon(const HWND hwnd) 
{
    if (strlen(exePath) > 0 && strlen(icoPath) > 0) 
    {
        icon_changer::change_icon(icoPath, exePath);
        MessageBoxA(hwnd, GUI::Text::SUCCESS_MSG, GUI::Text::RESULT_TITLE, MB_OK);
    } 
    else 
    {
        MessageBoxA(hwnd, GUI::Text::ERROR_MSG, GUI::Text::ERROR_TITLE, MB_OK);
    }
}

static void CreateWindowControls(const HWND hwnd) 
{
    // Create labels
    CreateLabel(hwnd, GUI::Text::EXE_LABEL, GUI::Layout::MARGIN_X, GUI::Layout::FIRST_ROW_Y);
    CreateLabel(hwnd, GUI::Text::ICO_LABEL, GUI::Layout::MARGIN_X, GUI::Layout::SECOND_ROW_Y);

    // Create buttons
    CreateButton(hwnd, GUI::Text::BROWSE_TEXT, GUI::Layout::BUTTON_OFFSET_X, GUI::Layout::FIRST_ROW_Y,
        GUI::Layout::BUTTON_WIDTH, GUI::Layout::BUTTON_HEIGHT, (HMENU)GUI::CHOOSE_EXE);
    CreateButton(hwnd, GUI::Text::BROWSE_TEXT, GUI::Layout::BUTTON_OFFSET_X, GUI::Layout::SECOND_ROW_Y,
        GUI::Layout::BUTTON_WIDTH, GUI::Layout::BUTTON_HEIGHT, (HMENU)GUI::CHOOSE_ICON);
    CreateButton(hwnd, GUI::Text::CHANGE_TEXT, GUI::Layout::MARGIN_X, GUI::Layout::THIRD_ROW_Y,
        GUI::Layout::CHANGE_BUTTON_WIDTH, GUI::Layout::CHANGE_BUTTON_HEIGHT, (HMENU)GUI::CHOOSE_CHANGE);
}

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) 
{
    switch (uMsg) 
    {
        case WM_CREATE:
            CreateWindowControls(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case GUI::CHOOSE_EXE:
                    HandlePathSelection(hwnd, exePath, GUI::Text::EXE_FILTER);
                    break;
                    
                case GUI::CHOOSE_ICON:
                    HandlePathSelection(hwnd, icoPath, GUI::Text::ICO_FILTER);
                    break;
                    
                case GUI::CHOOSE_CHANGE:
                    HandleChangeIcon(hwnd);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = GUI::CLASS_NAME;
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, GUI::CLASS_NAME, GUI::WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, GUI::WINDOW_WIDTH, GUI::WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

void LaunchGUI(void) 
{
    WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOW);
}
