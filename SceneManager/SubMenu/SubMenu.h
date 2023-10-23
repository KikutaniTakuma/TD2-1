#pragma once
#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Camera/Camera.h"


class SubMenu {
public:
	SubMenu();
	~SubMenu() = default;

	SubMenu(const SubMenu&) = delete;
	SubMenu(SubMenu&&) = delete;
	SubMenu& operator=(const SubMenu&) = delete;
	SubMenu& operator=(SubMenu&&) = delete;

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	void ActiveUpdate();
	virtual void Draw() = 0;

protected:
	virtual void Update() = 0;

public:
	UtilsLib::Flg isActive_;

protected:
	Camera camera_;
};