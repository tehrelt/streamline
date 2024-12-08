#include "combo-box.h"


ComboBox::ComboBox(HWND parent, int x, int y, int width, int height, int id) : UIElement(parent, id) {
    _handle = CreateWindow(
        "COMBOBOX", 
        NULL,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        x, y, width, height,
        _parent_handle,
		(HMENU)id,
        GetModuleHandle(NULL),
        NULL
    );
}

void ComboBox::add_item(const std::string& item) const {
    SendMessage(_handle, CB_ADDSTRING, 0, (LPARAM)item.c_str());
}

void ComboBox::remove_item(int index) {
    SendMessage(_handle, CB_DELETESTRING, index, 0);
}

std::string ComboBox::get_item(int index) {
    char buffer[256] = { 0 };
    SendMessage(_handle, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(buffer));
    return std::string(buffer);
}

std::string ComboBox::get_selected_item() {
    char buffer[256] = { 0 };
    SendMessage(_handle, CB_GETLBTEXT, get_selected_index(), reinterpret_cast<LPARAM>(buffer));
    return std::string(buffer);
}

int ComboBox::get_selected_index() {
    return static_cast<int>(SendMessage(_handle, CB_GETCURSEL, 0, 0));
}

void ComboBox::set_selected_index(int index) {
    SendMessage(_handle, CB_SETCURSEL, index, 0);
    on_change.notify(index);
}
