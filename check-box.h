#pragma once
#include "ui.h"
#include "observer.h"

class CheckBox : public UIElement
{
public:
	Subject<bool> on_check;

	CheckBox(int id, HWND parent, int x, int y, int w, int h) : UIElement(parent, id) {
		_handle = CreateWindow(
			"BUTTON",
			NULL,
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			x, y, w, h,
			parent,
			reinterpret_cast<HMENU>(id),
			GetModuleHandle(NULL),
			NULL
		);
	}

	bool is_checked() const {
		return SendMessage(_handle, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}

	void set_checked(bool checked) {
		SendMessage(_handle, BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
		on_check.notify(checked);
	}
};

