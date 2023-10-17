#include "UIeditor.h"
#include "externals/imgui/imgui.h"
#include "Engine/WinApp/WinApp.h"
#include "externals/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include "Input/Input.h"

UIeditor* UIeditor::GetInstance() {
	static UIeditor instance;
	return &instance;
}

void UIeditor::Update() {
	if (!ImGui::Begin("UIEditor", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	if (ImGui::Button("AddTex2D")) {
		isCreate = true;
		groupNameAdd.resize(32);
		fileName.resize(256);
	}

	if (isCreate) {
		ImGui::Begin("AddTex2D");
		ImGui::InputText("Name", groupNameAdd.data(), groupNameAdd.size());
		ImGui::InputText("FileName", fileName.data(), fileName.size());
		if (ImGui::Button("ok")) {
			std::string groupNameAddTmp;
			for (auto& i : groupNameAdd) {
				if (i == '\0') {
					break;
				}
				groupNameAddTmp += i;
			}
			CreateGroup(groupNameAddTmp);
			std::string fileNameTmp;
			for (auto& i : fileName) {
				if (i == '\0') {
					break;
				}
				fileNameTmp += i;
			}
			isCreate = !Add(fileNameTmp);
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel")) {
			isCreate = false;
		}
		ImGui::End();
	}

	for (auto itrGroup = datas.begin(); itrGroup != datas.end(); itrGroup++) {
		const std::string& groupName = itrGroup->first;
		auto& group = itrGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}
		for (auto& itemitr : group) {
			const std::string& itemName = itemitr.first;

			auto& item = itemitr.second;

			if (std::holds_alternative<int32_t>(item)) {
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item)) {
				float* ptr = std::get_if<float>(&item);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.01f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				Vector3* ptr = std::get_if<Vector3>(&item);
				if (itemName == "pos") {
					ImGui::DragFloat2(itemName.c_str(), &ptr->x, 1.0f);
				}
				else if (itemName == "scale" || itemName == "uvPibot" || itemName == "uvSize") {
					ImGui::DragFloat2(itemName.c_str(), &ptr->x, 0.01f);
				}
				else if (itemName == "rotate") {
					ImGui::DragFloat(itemName.c_str(), &ptr->z, 0.01f);
				}
				else if (itemName == "FileName") {
					continue;
				}
				else {
					ImGui::DragFloat3(itemName.c_str(), &ptr->x, 0.01f);
				}
			}
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();

	for (auto& i : textures) {
		i.second.pos = GetVector3Value(i.first, "pos");
		i.second.rotate = GetVector3Value(i.first, "rotate");
		auto scaleTmp = GetVector3Value(i.first, "scale");
		i.second.scale = { scaleTmp.x,scaleTmp.y };
		auto uvPibotTmp = GetVector3Value(i.first, "uvPibot");
		i.second.uvPibot = { uvPibotTmp.x,uvPibotTmp.y };
		auto uvSizeTmp = GetVector3Value(i.first, "uvSize");
		i.second.uvSize = { uvSizeTmp.x,uvSizeTmp.y };
	}
}

void UIeditor::Update(const Mat4x4& vpvpMat) {
	static auto mouse = Input::GetInstance()->GetMouse();

	Update();

	for (auto& tex : textures) {
		tex.second.Update();
	}

	Vector3 mousePos{ mouse->GetPos() };

	mousePos = MakeMatrixInverse(vpvpMat) * mousePos;
	
	for (auto tex = textures.begin(); tex != textures.end(); tex++) {
		if (tex->second.Collision({ mousePos.x, mousePos.y })) {
			if (mouse->Pushed(Mouse::Button::Left)) {
				texItr = tex;
				texItr->second.scale *= 1.2f;
			}
			else if(mouse->Releaed(Mouse::Button::Left)){
				if (texItr == textures.end()) {
					continue;
				}
				texItr->second.scale /= 1.2f;
				texItr = textures.end();
			}
			break;
		}
		else if(texItr == textures.end()){
			texItr = textures.end();
		}
	}

	if (texItr != textures.end()) {
		texItr->second.pos = mousePos;
		AddItem(texItr->first, "pos", texItr->second.pos);
		texItr->second.Update();
	}
}

void UIeditor::Draw(const Mat4x4& viewProjection) {
	for (auto& tex : textures) {
		tex.second.Draw(viewProjection, Pipeline::Blend::Normal);
	}
}

bool UIeditor::Add(const std::string& fileName_) {
	if (!std::filesystem::exists(fileName_)) {
		MessageBoxA(
			WinApp::GetInstance()->GetHwnd(),
			("No such a file : " + fileName_).c_str(), "UIEditor",
			MB_OK
		);
		return false;
	}

	std::string groupNameAddTmp;
	for (auto& i : groupNameAdd) {
		if (i == '\0') {
			break;
		}
		groupNameAddTmp += i;
	}

	Texture2D tmp;
	tmp.LoadTexture(fileName_);
	tmp.Initialize();

	if (textures.find(groupNameAddTmp) == textures.end()) {
		textures.insert({ groupNameAddTmp, std::move(tmp) });
	}
	else {
		MessageBoxA(
			WinApp::GetInstance()->GetHwnd(),
			("Already add Texture : " + fileName_).c_str(), "UIEditor",
			MB_OK
		);
		return false;
	}

	texItr = textures.end();

	AddItem(groupNameAddTmp, "scale", Vector3::identity);
	AddItem(groupNameAddTmp, "rotate", Vector3());
	AddItem(groupNameAddTmp, "pos", Vector3());
	AddItem(groupNameAddTmp, "uvPibot", Vector3());
	AddItem(groupNameAddTmp, "uvSize", Vector3::identity);
	AddItem(groupNameAddTmp, "FileName", fileName_);

	return true;
}

UIeditor::UIeditor():
	textures(0),
	isCreate(false)
{
	texItr = textures.end();
}

void UIeditor::LoadFile() {
	const std::filesystem::path kDirectoryPath = "./Datas/";

	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);
	for (const auto& entry : dirItr) {
		const auto& filePath = entry.path();

		auto extention = filePath.extension().string();
		if (extention.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());
	}
}

void UIeditor::LoadFile(const std::string& groupName) {
	const std::filesystem::path kDirectoryPath = "./Datas/";
	std::string filePath = kDirectoryPath.string() + groupName + std::string(".json");

	std::ifstream file(filePath);

	if (file.fail()) {
		return;
	}

	nlohmann::json root;

	file >> root;

	file.close();

	nlohmann::json::iterator groupItr = root.find(groupName);

	assert(groupItr != root.end());

	for (auto itemItr = groupItr->begin(); itemItr != groupItr->end(); itemItr++) {
		const std::string& itemName = itemItr.key();

		if (itemItr->is_number_integer()) {
			int32_t value = itemItr->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_number_float()) {
			float value = itemItr->get<float>();
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_array() && itemItr->size() == 3) {
			Vector3 value = { itemItr->at(0),itemItr->at(1),itemItr->at(2) };
			SetValue(groupName, itemName, value);
		}
		else if (itemItr->is_string()) {
			std::string value = itemItr->get<std::string>();
			SetValue(groupName, itemName, value);
		}
	}

	for (auto& i : datas) {
		textures[i.first];

		for (auto& j : i.second) {
			if (j.first == "FileName") {
				Texture2D tmp;
				tmp.LoadTexture(std::get<std::string>(j.second));
				tmp.Initialize();

				textures[i.first] = std::move(tmp);
			}
		}
	}

	for (auto& i : textures) {
		i.second.pos = GetVector3Value(i.first, "pos");
		i.second.rotate = GetVector3Value(i.first, "rotate");
		auto scaleTmp = GetVector3Value(i.first, "scale");
		i.second.scale = { scaleTmp.x,scaleTmp.y };
		auto uvPibotTmp = GetVector3Value(i.first, "uvPibot");
		i.second.uvPibot = { uvPibotTmp.x,uvPibotTmp.y };
		auto uvSizeTmp = GetVector3Value(i.first, "uvSize");
		i.second.uvSize = { uvSizeTmp.x,uvSizeTmp.y };
	}
}
