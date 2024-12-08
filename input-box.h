#pragma once
#include "ui.h"
#include "string"
#include "observer.h"

class InputBox : public UIElement {
public:
	Subject<std::string> on_change;

	InputBox(HWND parent, int x, int y, int width, int height, int id);
	void set_text(std::string text);
	std::string get_text();
};

