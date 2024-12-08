#include "ui.h"


UIElement::UIElement(HWND parent, int id) : _parent_handle(parent), _control_id(id) 
{ }

UIElement::~UIElement() {
	if (_handle) {
		DestroyWindow(_handle);
	}
}

