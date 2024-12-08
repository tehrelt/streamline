#pragma once
#include "observer.h"
#include "ui.h"
#include <Windows.h>
#include <string>
#include <mutex>

class Label : UIElement
{
public:
	Label(HWND hwnd, int x, int y, int w, int h, std::string init, int id) : UIElement(hwnd, id)  {
		_handle = CreateWindow(
			"STATIC", 
			init.c_str(), 
			WS_VISIBLE | WS_CHILD, 
			x, y, w, h, 
			hwnd, 
			nullptr, 
			GetModuleHandle(nullptr), 
			nullptr
		);
	}

	void set_text(std::string text) {
		SetWindowText(_handle, text.c_str());
	}
};

