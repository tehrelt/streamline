#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "observer.h"
#include "ui.h"

class ComboBox : public UIElement {
public:
    Subject<int> on_change;
    
    ComboBox(HWND parent, int x, int y, int width, int height, int id);

    void add_item(const std::string& item) const;
    void remove_item(int index);
    std::string get_item(int index);
    std::string get_selected_item();
    int get_selected_index();
    void set_selected_index(int index);
};

