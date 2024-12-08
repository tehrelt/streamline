#pragma once
#include "ui.h"
#include <windows.h>
#include <string>
#include <vector>

class ListBox : public UIElement {
public:
    ListBox(HWND parent, int x, int y, int width, int height, int id);

    void add_item(const std::string & item);
    void pop_item();
    void remove_item(int index);
    std::string get_item(int index) const;
    int selected_index() const;
    std::string selected_item() const;
    void clear();
private:

};

