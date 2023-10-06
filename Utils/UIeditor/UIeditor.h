#pragma once
#include <unordered_map>
#include <list>
#include "Drawers/Texture2D/Texture2D.h"
#include "GlobalVariables/GlobalVariables.h"

class UIeditor : public GlobalVariables {
private:
	UIeditor();
	UIeditor(const UIeditor&) = delete;
	UIeditor(UIeditor&&) noexcept = delete;
	~UIeditor() = default;

	UIeditor& operator=(const UIeditor&) = delete;
	UIeditor& operator=(UIeditor&&) noexcept = delete;

public:
	static UIeditor* GetInstance();

public:
	void Update() override;
	void Update(const Mat4x4& vpvpMat);

	void Draw(const Mat4x4& viewProjection);

	bool Add(const std::string& fileName);

	void LoadFile() override;
	void LoadFile(const std::string& groupName) override;

private:
	std::unordered_map<std::string, Texture2D> textures;
	decltype(textures)::iterator texItr;

	bool isCreate;
	std::string groupNameAdd;
	std::string fileName;
};