#include "SubMenu.h"

SubMenu::SubMenu():
	isActive_(false)
{}

void SubMenu::ActiveUpdate() {
	isActive_.Update();
	if (isActive_) {
		Update();
	}
}