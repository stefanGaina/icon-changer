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

/* NOTE: WIN32_LEAN_AND_MEAN must be defined before including <windows.h>.
   This excludes rarely used APIs from windows.h and reduces build time. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

namespace GUI
{
    // Window configuration
    static constexpr const char* const CLASS_NAME = "IconChangerWin32";
    static constexpr const char* const WINDOW_TITLE = "Icon Changer";
    static constexpr int WINDOW_WIDTH = 220;
    static constexpr int WINDOW_HEIGHT = 150;

    // Button IDs
    static constexpr UINT CHOOSE_EXE    = 1;  /* Search for executable button pressed */
    static constexpr UINT CHOOSE_ICON   = 2;  /* Search for an icon button pressed */
    static constexpr UINT CHOOSE_CHANGE = 3;  /* Change Icon button pressed */

    // Layout configuration
    namespace Layout 
    {
        static constexpr int LABEL_WIDTH = 70;
        static constexpr int LABEL_HEIGHT = 20;
        static constexpr int BUTTON_WIDTH = 100;
        static constexpr int BUTTON_HEIGHT = 20;
        static constexpr int CHANGE_BUTTON_WIDTH = 180;
        static constexpr int CHANGE_BUTTON_HEIGHT = 30;
        static constexpr int MARGIN_X = 10;
        static constexpr int FIRST_ROW_Y = 10;
        static constexpr int SECOND_ROW_Y = 40;
        static constexpr int THIRD_ROW_Y = 80;
        static constexpr int BUTTON_OFFSET_X = 90;
    }

    // Text constants and filters
    namespace Text 
    {
        static constexpr const char* const STATIC_CLASS = "STATIC";
        static constexpr const char* const BUTTON_CLASS = "BUTTON";
        static constexpr const char* const EXE_LABEL = "EXE Path:";
        static constexpr const char* const ICO_LABEL = "ICO Path:";
        static constexpr const char* const BROWSE_TEXT = "Browse...";
        static constexpr const char* const CHANGE_TEXT = "Change Icon";
        static constexpr const char* const SUCCESS_MSG = "Icon changed successfully!";
        static constexpr const char* const ERROR_MSG = "Please select both EXE and ICO files.";
        static constexpr const char* const RESULT_TITLE = "Result";
        static constexpr const char* const ERROR_TITLE = "Error";
        static constexpr const char* const EXE_FILTER = "Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
        static constexpr const char* const ICO_FILTER = "Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0";
    }
}


