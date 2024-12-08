#pragma once
#include <Windows.h>
#include <string>
#include <CommCtrl.h>
#include "observer.h"

#pragma comment(lib, "Comctl32.lib") 

class NumberInput {
    HWND _cur;
    HWND _hwnd;

    static LRESULT CALLBACK SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR subclassId, DWORD_PTR refData) {
        if (msg == WM_COMMAND && HIWORD(wParam) == EN_CHANGE) {
            NumberInput* self = reinterpret_cast<NumberInput*>(refData);
            self->handle_change();
        }
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    void handle_change() {
        char buffer[128];
        GetWindowTextA(_cur, buffer, sizeof(buffer));

        try {
            int value = std::stoi(buffer); // Convert text to integer
            on_change.notify(value);      // Notify observers
        } catch (const std::exception&) {
            // Handle invalid input if necessary
        }
    }

public:
    NumberInput(HINSTANCE inst, HWND hwnd, int x, int y, int w, int h, int init) {
        _hwnd = hwnd;
        _cur = CreateWindow("EDIT", NULL, WS_VISIBLE | WS_BORDER | WS_CHILD | ES_NUMBER, x, y, w, h, hwnd, nullptr, inst, nullptr);

        // Set initial value
        std::string initText = std::to_string(init);
        SetWindowTextA(_cur, initText.c_str());

        // Subclass the EDIT control
        SetWindowSubclass(_cur, SubclassProc, 1, reinterpret_cast<DWORD_PTR>(this));
    }

    Subject<int> on_change;

    ~NumberInput() {
        RemoveWindowSubclass(_cur, SubclassProc, 1);
    }
};
