#include "list-box.h"


ListBox::ListBox(HWND parent, int x, int y, int width, int height, int id) : UIElement(parent, id) {
    _handle = CreateWindow(
        "LISTBOX",
        NULL,
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL,
        x, y, width, height,
        parent,
        reinterpret_cast<HMENU>(id),
        GetModuleHandle(NULL),
        NULL
    );
}


void ListBox::add_item(const std::string & item) {
    SendMessage(_handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
}

void ListBox::pop_item() {
    SendMessage(_handle, LB_DELETESTRING, 0, 0);
}

void ListBox::remove_item(int index) {
    SendMessage(_handle, LB_DELETESTRING, index, 0);
}

std::string ListBox::get_item(int index) const {
    int length = SendMessage(_handle, LB_GETTEXTLEN, index, 0);
    std::string item(length, L'\0');
    SendMessage(_handle, LB_GETTEXT, index, reinterpret_cast<LPARAM>(&item[0]));
    return item;
}

int ListBox::selected_index() const {
    return SendMessageW(_handle, LB_GETCURSEL, 0, 0);
}

std::string ListBox::selected_item() const {
    int index = selected_index();
    return (index != LB_ERR) ? get_item(index) : "";
}

void ListBox::clear() {
    SendMessageW(_handle, LB_RESETCONTENT, 0, 0);
}
