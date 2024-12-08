#pragma once
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib") // Link with the Common Controls Library

class ProgressBar : public UIElement{
private:
    int minValue;      // Minimum value of the progress bar
    int maxValue;      // Maximum value of the progress bar

public:
    ProgressBar(int id, HWND parent, int x, int y, int width, int height, int min = 0, int max = 100)
        : UIElement(parent, id) {
        InitCommonControls(); // Ensure common controls are initialized

        _handle = CreateWindowEx(
            0, PROGRESS_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE,
            x, y, width, height,
            parent, nullptr, GetModuleHandle(nullptr), nullptr
        );
    }

    void set_position(int position) {
        if (_handle) {
            SendMessage(_handle, PBM_SETPOS, position, 0);
        }
    }

    void increment(int value) {
        if (_handle) {
            SendMessage(_handle, PBM_DELTAPOS, value, 0);
        }
    }

    void set_color(COLORREF color) {
        if (_handle) {
            SendMessage(_handle, PBM_SETBARCOLOR, 0, static_cast<LPARAM>(color));
        }
    }

    int get_position() {
        if (_handle) {
            return static_cast<int>(SendMessage(_handle, PBM_GETPOS, 0, 0));
        }
        return -1;
    }
};


