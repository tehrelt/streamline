#pragma once
#include "ui.h"
#include "string"
#include <unordered_map>

class ContextMenu : public UIElement
{
private:
	std::unordered_map<int, std::string> items;

	HMENU create(HWND hwnd) {
		auto menu = CreatePopupMenu();

		for (auto pair : items) {
			AppendMenu(menu, MF_STRING, pair.first, pair.second.c_str());
		}

		return menu;
	}

public:
	ContextMenu(int id, HWND parent) : UIElement(parent, id) { }

	void add_item(int id, std::string message) {
		items[id] = message;
	}

	void enable(HWND hwnd) {
		auto _menu = create(hwnd);
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(_menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
		DestroyMenu(_menu);
	}
};

