#pragma once
#include "Windows.h"

class UIElement
{
public:
    UIElement(HWND parent, int id);
    ~UIElement();

    HWND handle() const { return _handle; }
    int id() const { return _control_id; }

protected:
    HWND _handle;         
    HWND _parent_handle;   
    int _control_id;       
};

