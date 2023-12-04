#include "SubMenu.h"

SubMenu::SubMenu():
	isActive_(false),
	camera_(new Camera{})
{}

void SubMenu::ActiveUpdate() {
	isActive_.Update();
	if (isActive_) {
		Update();
	}
}