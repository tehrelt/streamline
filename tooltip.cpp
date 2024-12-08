#include "tooltip.h"

Tooltip::Tooltip(HWND parent, HWND target, int id, std::string init_text) : UIElement(parent, id) {
	_target = target;
	_handle = CreateWindowEx(
		WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		parent,
		NULL, NULL, NULL
	);

	_info = { 0 };
	_info.cbSize = TTTOOLINFO_V1_SIZE;
	_info.hwnd = parent;
	_info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	_info.uId = (UINT_PTR)target;
	_info.lpszText = (LPSTR)init_text.c_str();

	SendMessage(_handle, TTM_ADDTOOL, 0, (LPARAM)&_info);
	SendMessage(_handle, TTM_SETMAXTIPWIDTH, 0, 300);
}

