#include "input-box.h"

InputBox::InputBox(HWND parent, int x, int y, int width, int height, int id) : UIElement(parent, id) {
	_handle = CreateWindow(
        "EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        x, y, width, height,
        parent,
        reinterpret_cast<HMENU>(id),
        GetModuleHandle(NULL),
        NULL
    );
}

void InputBox::set_text(std::string text) {
    SetWindowText(_handle, text.c_str());
    on_change.notify(text);
}

std::string InputBox::get_text() {
	int length = GetWindowTextLengthW(_handle);
    std::string text(length, '\0');
    GetWindowText(_handle, &text[0], length + 1);
    return text;
}


