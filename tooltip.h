#pragma once
#include "ui.h"
#include "string"
#include <CommCtrl.h>

class Tooltip : public UIElement
{
private:
	TOOLINFO _info;
	HWND _target;

public:
	Tooltip(HWND parent, HWND target, int id, std::string init_text);
};

