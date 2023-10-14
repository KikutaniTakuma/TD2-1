#include "Particle.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "Engine/WinApp/WinApp.h"
#include <numeric>

#include "externals/nlohmann/json.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>

/// <summary>
/// 静的変数のインスタンス化
/// </summary>

std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Particle::graphicsPipelineState = {};
Shader Particle::shader = {};

D3D12_INDEX_BUFFER_VIEW Particle::indexView = {};
Microsoft::WRL::ComPtr<ID3D12Resource> Particle::indexResource = nullptr;

void Particle::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	if (indexResource) { indexResource->Release(); }

	LoadShader(vsFileName, psFileName);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource = Engine::CreateBufferResuorce(sizeof(indices));
	indexView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexView.SizeInBytes = sizeof(indices);
	indexView.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource->Unmap(0, nullptr);

	CreateGraphicsPipeline();
}

void Particle::Finalize() {
	if (indexResource) {
		indexResource->Release();
		indexResource.Reset();
	}
}

void Particle::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void Particle::CreateGraphicsPipeline() {
	D3D12_DESCRIPTOR_RANGE range = {};
	range.NumDescriptors = 3;
	range.BaseShaderRegister = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_ROOT_PARAMETER, 1> rootPrams={};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = 1;
	rootPrams[0].DescriptorTable.pDescriptorRanges = &range;
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true);
	PipelineManager::SetShader(shader);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	PipelineManager::IsDepth(false);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineState[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();
}

Particle::Particle(uint32_t indexNum) :
	wtfs(),
	uvPibot(),
	uvSize(Vector2::identity),
	tex(nullptr),
	isLoad(false),
	wvpMat(indexNum),
	colorBuf(indexNum),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f),
	settings(),
	currentSettingIndex(0u),
	currentParticleIndex(0u)
{
	for (uint32_t i = 0; i < wvpMat.Size();i++) {
		wvpMat[i] = MakeMatrixIndentity();
	}
	
	for (uint32_t i = 0; i < colorBuf.Size(); i++) {
		colorBuf[i] = Vector4::identity;
	}

	if (vertexResource) { vertexResource->Release(); }

	vertexResource = Engine::CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 4;
	vertexView.StrideInBytes = sizeof(VertexData);

	wtfs.resize(indexNum);

	for (size_t i = 0; i < wtfs.size();i++) {
		wtfs[i].scale = Vector2::identity * 512.0f;
		wtfs[i].pos.x = 10.0f * i;
		wtfs[i].pos.y = 10.0f * i;
		wtfs[i].pos.z += 0.3f;
	}
}

Particle::Particle(const Particle& right) :
	Particle(right.wvpMat.Size())
{
	*this = right;
}
Particle::Particle(Particle&& right) noexcept :
	Particle(right.wvpMat.Size())
{
	*this = std::move(right);
}

Particle& Particle::operator=(const Particle& right) {
	assert(wvpMat.Size() == right.wvpMat.Size());

	wtfs = right.wtfs;

	uvPibot = right.uvPibot;
	uvSize = right.uvSize;

	tex = right.tex;
	isLoad = right.isLoad;

	for (uint32_t i = 0; i < wvpMat.Size();i++) {
		wvpMat[i] = right.wvpMat[i];
	}
	
	for (uint32_t i = 0; i < colorBuf.Size(); i++) {
		colorBuf[i] = right.colorBuf[i];
	}

	aniStartTime_ = right.aniStartTime_;
	aniCount_ = right.aniCount_;
	isAnimation_ = right.isAnimation_;
	uvPibotSpd_ = right.uvPibotSpd_;

	return *this;
}

Particle& Particle::operator=(Particle&& right) noexcept {
	wtfs = std::move(right.wtfs);

	uvPibot = std::move(right.uvPibot);
	uvSize = std::move(right.uvSize);

	tex = std::move(right.tex);
	isLoad = std::move(right.isLoad);

	for (uint32_t i = 0; i < wvpMat.Size(); i++) {
		wvpMat[i] = right.wvpMat[i];
	}
	for (uint32_t i = 0; i < colorBuf.Size(); i++) {
		colorBuf[i] = right.colorBuf[i];
	}

	aniStartTime_ = std::move(right.aniStartTime_);
	aniCount_ = std::move(right.aniCount_);
	isAnimation_ = std::move(right.isAnimation_);
	uvPibotSpd_ = std::move(right.uvPibotSpd_);

	return *this;
}

Particle::~Particle() {
	if (vertexResource) {
		vertexResource->Release();
		indexResource.Reset();
	}
}


void Particle::LoadTexture(const std::string& fileName) {
	tex = TextureManager::GetInstance()->LoadTexture(fileName);

	if (tex && !isLoad) {
		isLoad = true;
	}
	srvHeap.InitializeReset(3);
	srvHeap.CreateTxtureView(tex);
	srvHeap.CreateStructuredBufferView(wvpMat);
	srvHeap.CreateStructuredBufferView(colorBuf);
}

void Particle::ThreadLoadTexture(const std::string& fileName) {
	tex = nullptr;
	TextureManager::GetInstance()->LoadTexture(fileName, &tex);
	isLoad = false;
}


void Particle::LopadSettingDirectory(const std::string& directoryName) {
	const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/" + directoryName + "/";
	dataDirectoryName = kDirectoryPath.string();

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directories(kDirectoryPath);
	}

	std::filesystem::directory_iterator dirItr(kDirectoryPath);
	
	// directory内のファイルをすべて読み込む
	for (const auto& entry : dirItr) {
		const auto& filePath = entry.path();


		// jsonファイルじゃなかったら飛ばす
		if (filePath.extension() != L".json") {
			continue;
		}

		// jsonファイルを読み込む
		LopadSettingFile(filePath.string());
	}

	std::ifstream file{ dataDirectoryName + "loop.txt" };

	if (!file.fail()) {
		bool isLoop = false;
		file >> isLoop;
		isLoop_ = isLoop;
		file.close();
	}
}

void Particle::LopadSettingFile(const std::string& jsonName) {
	std::ifstream file(jsonName);

	if (file.fail()) {
		return;
	}

	nlohmann::json root;

	file >> root;

	file.close();

	std::filesystem::path groupName = jsonName;
	groupName = groupName.stem();

	nlohmann::json::iterator groupItr = root.find(groupName);

	assert(groupItr != root.end());

	for (auto itemItr = groupItr->begin(); itemItr != groupItr->end(); itemItr++) {
		const std::string& itemName = itemItr.key();

		if (itemItr->is_number_integer()) {
			uint32_t value = itemItr->get<uint32_t>();
			auto& gruop = datas[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_number_float()) {
			float value = itemItr->get<float>();
			auto& gruop = datas[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_array() && itemItr->size() == 2) {
			Vector2 value = { itemItr->at(0),itemItr->at(1) };
			auto& gruop = datas[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_array() && itemItr->size() == 3) {
			Vector3 value = { itemItr->at(0),itemItr->at(1),itemItr->at(2) };
			auto& gruop = datas[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
		else if (itemItr->is_string()) {
			std::string value = itemItr->get<std::string>();
			auto& gruop = datas[groupName.string()];
			Item item = value;
			gruop[itemName] = value;
		}
	}

	settings.push_back(Setting{});
	auto& setting = settings.back();

	setting.emitter.pos = std::get<Vector3>(datas[groupName.string()]["Emitter_Pos"]);
	setting.emitter.size = std::get<Vector3>(datas[groupName.string()]["Emitter_Size"]);
	setting.emitter.type = static_cast<EmitterType>(std::get<uint32_t>(datas[groupName.string()]["Emitter_Type"]));
	setting.emitter.circleSize = std::get<float>(datas[groupName.string()]["Emitter_CircleSize"]);
	setting.emitter.rotate.first = std::get<Vector3>(datas[groupName.string()]["Emitter_RotateFirst"]);
	setting.emitter.rotate.second = std::get<Vector3>(datas[groupName.string()]["Emitter_RotateSecond"]);
	setting.emitter.particleMaxNum = std::get<uint32_t>(datas[groupName.string()]["Emitter_ParticleMaxNum"]);
	setting.emitter.validTime = std::chrono::milliseconds(std::get<uint32_t>(datas[groupName.string()]["Emitter_vaildTime"]));

	setting.size.first = std::get<Vector2>(datas[groupName.string()]["Particle_sizeFirst"]);
	setting.size.second = std::get<Vector2>(datas[groupName.string()]["Particle_sizeSecond"]);
	setting.velocity.first = std::get<Vector3>(datas[groupName.string()]["Particle_velocityFirst"]);
	setting.velocity.second = std::get<Vector3>(datas[groupName.string()]["Particle_velocitySecond"]);
	setting.rotate.first = std::get<Vector3>(datas[groupName.string()]["Particle_rotateFirst"]);
	setting.rotate.second = std::get<Vector3>(datas[groupName.string()]["Particle_rotateSecond"]);
	setting.particleNum.first = std::get<uint32_t>(datas[groupName.string()]["Particle_particleNumFirst"]);
	setting.particleNum.second = std::get<uint32_t>(datas[groupName.string()]["Particle_particleNumSecond"]);
	setting.freq.first = std::get<uint32_t>(datas[groupName.string()]["Particle_freqFirst"]);
	setting.freq.second = std::get<uint32_t>(datas[groupName.string()]["Particle_freqSecond"]);
	setting.death.first = std::get<uint32_t>(datas[groupName.string()]["Particle_deathFirst"]);
	setting.death.second = std::get<uint32_t>(datas[groupName.string()]["Particle_deathSecond"]);
	setting.color.first = std::get<uint32_t>(datas[groupName.string()]["Particle_colorFirst"]);
	setting.color.second = std::get<uint32_t>(datas[groupName.string()]["Particle_colorSecond"]);
}

void Particle::SaveSettingFile(const std::string& groupName) {
	auto itrGroup = datas.find(groupName);
	assert(itrGroup != datas.end());

	nlohmann::json root;

	root = nlohmann::json::object();

	root[groupName] = nlohmann::json::object();

	for (auto itemItr = itrGroup->second.begin(); itemItr != itrGroup->second.end(); itemItr++) {
		const std::string& itemName = itemItr->first;

		auto& item = itemItr->second;

		if (std::holds_alternative<uint32_t>(item)) {
			root[groupName][itemName] = std::get<uint32_t>(item);
		}
		else if (std::holds_alternative<float>(item)) {
			root[groupName][itemName] = std::get<float>(item);
		}
		else if (std::holds_alternative<Vector2>(item)) {
			auto tmp = std::get<Vector2>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y });
		}
		else if (std::holds_alternative<Vector3>(item)) {
			auto tmp = std::get<Vector3>(item);
			root[groupName][itemName] = nlohmann::json::array({ tmp.x, tmp.y, tmp.z });
		}
		else if (std::holds_alternative<std::string>(item)) {
			root[groupName][itemName] = std::get<std::string>(item);
		}
	}

	const std::filesystem::path kDirectoryPath = dataDirectoryName;

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}

	std::string groupNameTmp;
	for (auto& i : groupName) {
		if (i == '\0') {
			break;
		}
		groupNameTmp += i;
	}
	auto filePath = (kDirectoryPath.string() + groupNameTmp) + std::string(".json");

	std::ofstream file(filePath);

	if (file.fail()) {
		assert(!"fileSaveFailed");
		return;
	}

	file << std::setw(4) << root << std::endl;

	file.close();
}


void Particle::Update() {
	assert(wtfs.size() == wvpMat.Size());

	if (tex && tex->CanUse() && !isLoad) {
		srvHeap.InitializeReset(3);
		srvHeap.CreateTxtureView(tex);
		srvHeap.CreateStructuredBufferView(wvpMat);
		srvHeap.CreateStructuredBufferView(colorBuf);
		isLoad = true;
	}

	// currentSettingIndexがsettingの要素数を超えてたらassertで止める
	if (currentSettingIndex >= settings.size() || settings.empty()) {
		return;
	}
	auto nowTime = std::chrono::steady_clock::now();


	// 有効になった瞬間始めた瞬間を保存
	if (settings[currentSettingIndex].isValid.OnEnter()) {
		settings[currentSettingIndex].startTime = nowTime;
		settings[currentSettingIndex].durationTime = nowTime;
		// パーティクルの最大数を保存
		this->Resize(settings[currentSettingIndex].emitter.particleMaxNum);
	}
	// 有効中
	else if (settings[currentSettingIndex].isValid.OnStay()) {
		// 最後に出した時間からのmilliseconds
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - settings[currentSettingIndex].durationTime);

		// 出す頻度ランダム
		auto freq = UtilsLib::Random(settings[currentSettingIndex].freq.first, settings[currentSettingIndex].freq.second);

		// 頻度時間を超えてたら
		if (duration > decltype(duration)(freq)) {
			settings[currentSettingIndex].durationTime = nowTime;

			// パーティクルを出す数ランダム
			auto particleNum = UtilsLib::Random(settings[currentSettingIndex].particleNum.first, settings[currentSettingIndex].particleNum.second);

			// パーティクルの設定
			for (uint32_t i = currentParticleIndex; i < currentParticleIndex + particleNum; i++) {
				if (i >= wtfs.size()) {
					currentParticleIndex = 0u;
					break;
				}

				// ポジションランダム
				Vector3 maxPos = settings[currentSettingIndex].emitter.pos + settings[currentSettingIndex].emitter.size;
				Vector3 minPos = settings[currentSettingIndex].emitter.pos - settings[currentSettingIndex].emitter.size;
				[[maybe_unused]] Vector3 posRotate;
				Vector3 pos;

				// ポジションのランダム
				switch (settings[currentSettingIndex].emitter.type)
				{
				case Particle::EmitterType::Cube:
				default:
					pos = UtilsLib::Random(minPos, maxPos);
					break;

				case Particle::EmitterType::Circle:
					maxPos.x += settings[currentSettingIndex].emitter.circleSize;
					pos = UtilsLib::Random(settings[currentSettingIndex].emitter.pos, maxPos);
					posRotate = UtilsLib::Random(settings[currentSettingIndex].emitter.rotate.first, settings[currentSettingIndex].emitter.rotate.second);
					pos *= HoriMakeMatrixAffin(Vector3::identity, posRotate, Vector3::zero);
					break;
				}

				// 大きさランダム
				Vector2 size = UtilsLib::Random(settings[currentSettingIndex].size.first, settings[currentSettingIndex].size.second);

				// 速度ランダム
				Vector3 velocity = UtilsLib::Random(settings[currentSettingIndex].velocity.first, settings[currentSettingIndex].velocity.second);

				// 移動方向ランダム
				Vector3 rotate = UtilsLib::Random(settings[currentSettingIndex].rotate.first, settings[currentSettingIndex].rotate.second);

				// 速度回転
				velocity *= HoriMakeMatrixAffin(Vector3::identity, rotate, Vector3::zero);

				// 死ぬ時間ランダム
				uint32_t deathTime = UtilsLib::Random(settings[currentSettingIndex].death.first, settings[currentSettingIndex].death.second);

				// カラー(今後ラープさせる予定)
				uint32_t color = settings[currentSettingIndex].color.first;

				// ステータスセット
				wtfs[i].pos = pos;
				wtfs[i].scale = size;
				wtfs[i].movePos = velocity;
				wtfs[i].deathTime = std::chrono::milliseconds(deathTime);
				wtfs[i].startTime = nowTime;
				wtfs[i].isActive = true;
				wtfs[i].color = color;
			}

			// インデックスを更新
			currentParticleIndex = currentParticleIndex + particleNum;
			if (currentParticleIndex >= wtfs.size()) {
				currentParticleIndex = 0u;
			}
		}
	}


	for (auto& wtf : wtfs) {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - wtf.startTime);
		if (wtf.isActive) {
			if (duration > wtf.deathTime) {
				wtf.isActive = false;
			}
			else {
				float colorT =static_cast<float>(duration.count()) / static_cast<float>(wtf.deathTime.count());
				wtf.pos += wtf.movePos * FrameInfo::GetInstance()->GetDelta();
				wtf.color = ColorLerp(settings[currentSettingIndex].color.first, settings[currentSettingIndex].color.second, colorT);
			}
		}

	}

	settings[currentSettingIndex].isValid.Update();

	// もし今の設定の有効時間を過ぎていたら終了
	if (settings[currentSettingIndex].isValid && 
		settings[currentSettingIndex].emitter.validTime <
		std::chrono::duration_cast<std::chrono::milliseconds>(
			nowTime - settings[currentSettingIndex].startTime
		))
	{
		settings[currentSettingIndex].isValid = false;

		currentSettingIndex++;
		if (currentSettingIndex >= settings.size()) {
			currentSettingIndex = 0;
			if (isLoop_) {
				settings[currentSettingIndex].isValid = true;
			}
		}
	}
}

void Particle::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend
) {
	if (tex && isLoad && !settings.empty() && settings[currentSettingIndex].isValid) {
		const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
		const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

		std::array<VertexData, 4> pv = {
			Vector3{ -0.5f,  0.5f, 0.1f }, uv3,
			Vector3{  0.5f,  0.5f, 0.1f }, uv2,
			Vector3{  0.5f, -0.5f, 0.1f }, uv1,
			Vector3{ -0.5f, -0.5f, 0.1f }, uv0
		};

		VertexData* mappedData = nullptr;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource->Unmap(0, nullptr);

		for (uint32_t i = 0; i < wvpMat.Size();i++) {
			if (wtfs[i].isActive) {
				wvpMat[i] = viewProjection * VertMakeMatrixAffin(wtfs[i].scale, wtfs[i].rotate, wtfs[i].pos);
				colorBuf[i] = UintToVector4(wtfs[i].color);
			}
			else {
				wvpMat[i] = Mat4x4();
				colorBuf[i] = UintToVector4(0u);
			}
		}

		auto commandlist = Engine::GetCommandList();

		for (auto& i : graphicsPipelineState) {
			if (!i) {
				ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Particle");
				return;
			}
		}

		// 各種描画コマンドを積む
		graphicsPipelineState[blend]->Use();
		srvHeap.Use();
		commandlist->IASetVertexBuffers(0, 1, &vertexView);
		commandlist->IASetIndexBuffer(&indexView);
		commandlist->DrawIndexedInstanced(6, wvpMat.Size(), 0, 0, 0);
	}
}

void Particle::Debug(const std::string& guiName) {
	if (!ImGui::Begin(guiName.c_str(), nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar()) {
		return;
	}

	if (ImGui::BeginMenu("Load")) {
		const std::filesystem::path kDirectoryPath = "./Resources/Datas/Particles/";

		if (std::filesystem::exists(kDirectoryPath)) {
			std::filesystem::directory_iterator dirItr(kDirectoryPath);

			// directory内のファイルをすべて読み込む
			for (const auto& entry : dirItr) {
				if (ImGui::Button(entry.path().string().c_str())) {
					LopadSettingDirectory(entry.path().string());
				}
			}
		}

		ImGui::EndMenu();
	}
	if (ImGui::Button("Setting Add")) {
		settings.push_back(Setting{});

	}
	for (auto i = 0llu; i < settings.size(); i++) {
		if (!ImGui::BeginMenu(("setting" + std::to_string(i)).c_str())) {
			continue;
		}

		// エミッターの設定
		if (ImGui::TreeNode("Emitter")) {
			ImGui::DragFloat3("pos", &settings[i].emitter.pos.x, 0.01f);
			ImGui::DragFloat3("size", &settings[i].emitter.size.x, 0.01f);
			int32_t type = static_cast<int32_t>(settings[i].emitter.type);
			ImGui::SliderInt("type", &type, 0, 1);
			settings[i].emitter.type = static_cast<decltype(settings[i].emitter.type)>(type);
			if (type == 1) {
				ImGui::DragFloat("circleSize", &settings[i].emitter.circleSize, 0.01f);
				ImGui::DragFloat3("rotate first", &settings[i].emitter.rotate.first.x, 0.01f);
				ImGui::DragFloat3("rotate second", &settings[i].emitter.rotate.second.x, 0.01f);
			}
			int32_t particleMaxNum = static_cast<int32_t>(settings[i].emitter.particleMaxNum);
			ImGui::DragInt("particleMaxNum", &particleMaxNum, 0.1f, 0);
			settings[i].emitter.particleMaxNum = uint32_t(particleMaxNum);
			settings[i].emitter.particleMaxNum = std::clamp(settings[i].emitter.particleMaxNum, 1u, std::numeric_limits<uint32_t>::max());

			int32_t validTime = int32_t(settings[i].emitter.validTime.count());
			ImGui::DragInt("vaild time(milliseconds)", &validTime, 1.0f, 0);
			settings[i].emitter.validTime = std::chrono::milliseconds(validTime);

			ImGui::TreePop();
		}

		// パーティクルの設定
		if (ImGui::TreeNode("Particle")) {
			ImGui::DragFloat2("size first", &settings[i].size.first.x, 0.01f);
			ImGui::DragFloat2("size second", &settings[i].size.second.x, 0.01f);
			ImGui::DragFloat3("velocity first", &settings[i].velocity.first.x, 0.01f);
			ImGui::DragFloat3("velocity second", &settings[i].velocity.second.x, 0.01f);
			ImGui::DragFloat3("rotate first", &settings[i].rotate.first.x, 0.01f);
			ImGui::DragFloat3("rotate second", &settings[i].rotate.second.x, 0.01f);

			auto particleNumFirst = int32_t(settings[i].particleNum.first);
			auto particleNumSecond = int32_t(settings[i].particleNum.second);
			ImGui::DragInt("particleNum first", &particleNumFirst, 1.0f);
			ImGui::DragInt("particleNum second", &particleNumSecond, 1.0f);
			settings[i].particleNum.first = uint32_t(particleNumFirst);
			settings[i].particleNum.second = uint32_t(particleNumSecond);

			auto freqFirst = int32_t(settings[i].freq.first);
			auto freqSecond = int32_t(settings[i].freq.second);
			ImGui::DragInt("freq first", &freqFirst, 1.0f);
			ImGui::DragInt("freq second", &freqSecond, 1.0f);
			settings[i].freq.first = uint32_t(freqFirst);
			settings[i].freq.second = uint32_t(freqSecond);

			auto deathFirst = int32_t(settings[i].death.first);
			auto deathSecond = int32_t(settings[i].death.second);
			ImGui::DragInt("death first(milliseconds)", &deathFirst, 10.0f);
			ImGui::DragInt("death second(milliseconds)", &deathSecond, 10.0f);
			settings[i].death.first = uint32_t(deathFirst);
			settings[i].death.second = uint32_t(deathSecond);

			Vector4 colorFirst = UintToVector4(settings[i].color.first);
			Vector4 colorSecond = UintToVector4(settings[i].color.second);
			ImGui::ColorEdit4("color first", colorFirst.m.data());
			ImGui::ColorEdit4("color second", colorSecond.m.data());
			settings[i].color.first = Vector4ToUint(colorFirst);
			settings[i].color.second = Vector4ToUint(colorSecond);

			ImGui::TreePop();
		}

		if (ImGui::Button("start")) {
			settings[i].isValid = true;
			currentParticleIndex = uint32_t(i);
		}
		if (ImGui::Button("stop")) {
			settings[i].isValid = false;
		}

		const auto groupName = ("setting" + std::to_string(i));

		datas[groupName]["Emitter_Pos"] = settings[i].emitter.pos;
		datas[groupName]["Emitter_Size"] = settings[i].emitter.size;
		datas[groupName]["Emitter_Type"] = static_cast<uint32_t>(settings[i].emitter.type);
		datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
		datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
		datas[groupName]["Emitter_RotateFirst"] = settings[i].emitter.rotate.first;
		datas[groupName]["Emitter_RotateSecond"] = settings[i].emitter.rotate.second;
		datas[groupName]["Emitter_ParticleMaxNum"] = settings[i].emitter.particleMaxNum;
		datas[groupName]["Emitter_vaildTime"] = static_cast<uint32_t>(settings[i].emitter.validTime.count());

		datas[groupName]["Particle_sizeFirst"] = settings[i].size.first;
		datas[groupName]["Particle_sizeSecond"] = settings[i].size.second;
		datas[groupName]["Particle_velocityFirst"] = settings[i].velocity.first;
		datas[groupName]["Particle_velocitySecond"] = settings[i].velocity.second;
		datas[groupName]["Particle_rotateFirst"] = settings[i].rotate.first;
		datas[groupName]["Particle_rotateSecond"] = settings[i].rotate.second;
		datas[groupName]["Particle_particleNumFirst"] = settings[i].particleNum.first;
		datas[groupName]["Particle_particleNumSecond"] = settings[i].particleNum.second;
		datas[groupName]["Particle_freqFirst"] = settings[i].freq.first;
		datas[groupName]["Particle_freqSecond"] = settings[i].freq.second;
		datas[groupName]["Particle_deathFirst"] = settings[i].death.first;
		datas[groupName]["Particle_deathSecond"] = settings[i].death.second;
		datas[groupName]["Particle_colorFirst"] = settings[i].color.first;
		datas[groupName]["Particle_colorSecond"] = settings[i].color.second;


		if (ImGui::Button("this setting save")) {
			SaveSettingFile(("setting" + std::to_string(i)).c_str());
			MessageBoxA(
				WinApp::GetInstance()->GetHwnd(),
				"save success", "Particle",
				MB_OK
			);
		}


		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();

	ImGui::Checkbox("isLoop", isLoop_.Data());
	if (ImGui::Button("all setting save")) {
		for (auto i = 0llu; i < settings.size(); i++) {
			const auto groupName = ("setting" + std::to_string(i));

			datas[groupName]["Emitter_Pos"] = settings[i].emitter.pos;
			datas[groupName]["Emitter_Size"] = settings[i].emitter.size;
			datas[groupName]["Emitter_Type"] = static_cast<uint32_t>(settings[i].emitter.type);
			datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
			datas[groupName]["Emitter_CircleSize"] = settings[i].emitter.circleSize;
			datas[groupName]["Emitter_RotateFirst"] = settings[i].emitter.rotate.first;
			datas[groupName]["Emitter_RotateSecond"] = settings[i].emitter.rotate.second;
			datas[groupName]["Emitter_ParticleMaxNum"] = settings[i].emitter.particleMaxNum;
			datas[groupName]["Emitter_vaildTime"] = static_cast<uint32_t>(settings[i].emitter.validTime.count());

			datas[groupName]["Particle_sizeFirst"] = settings[i].size.first;
			datas[groupName]["Particle_sizeSecond"] = settings[i].size.second;
			datas[groupName]["Particle_velocityFirst"] = settings[i].velocity.first;
			datas[groupName]["Particle_velocitySecond"] = settings[i].velocity.second;
			datas[groupName]["Particle_rotateFirst"] = settings[i].rotate.first;
			datas[groupName]["Particle_rotateSecond"] = settings[i].rotate.second;
			datas[groupName]["Particle_particleNumFirst"] = settings[i].particleNum.first;
			datas[groupName]["Particle_particleNumSecond"] = settings[i].particleNum.second;
			datas[groupName]["Particle_freqFirst"] = settings[i].freq.first;
			datas[groupName]["Particle_freqSecond"] = settings[i].freq.second;
			datas[groupName]["Particle_deathFirst"] = settings[i].death.first;
			datas[groupName]["Particle_deathSecond"] = settings[i].death.second;
			datas[groupName]["Particle_colorFirst"] = settings[i].color.first;
			datas[groupName]["Particle_colorSecond"] = settings[i].color.second;
			SaveSettingFile(("setting" + std::to_string(i)).c_str());
		}

		std::ofstream file{ dataDirectoryName+"loop.txt"};

		if (!file.fail()) {
			file << static_cast<bool>(isLoop_);
			file.close();
			MessageBoxA(
				WinApp::GetInstance()->GetHwnd(),
				"save success", "Particle",
				MB_OK
			);
		}
	}

	ImGui::End();
}

void Particle::AnimationStart(float aniUvPibot) {
	if (!isAnimation_) {
		aniStartTime_ = std::chrono::steady_clock::now();
		isAnimation_ = true;
		aniCount_ = 0.0f;
		uvPibot.x = aniUvPibot;
	}
}

void Particle::AnimationPause() {
	isAnimation_ = false;
}

void Particle::AnimationRestart() {
	isAnimation_ = true;
}

void Particle::Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd) {
	if (isAnimation_) {
		auto nowTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - aniStartTime_) >= std::chrono::milliseconds(aniSpd)) {
			aniStartTime_ = nowTime;
			aniCount_++;

			if (aniCount_ >= aniUvEnd) {
				if (isLoop) {
					aniCount_ = aniUvStart;
				}
				else {
					--aniCount_;
					isAnimation_ = false;
				}
			}

			uvPibot.x = aniUvStart;
			uvPibot.x += uvPibotSpd_ * aniCount_;
		}
	}
}