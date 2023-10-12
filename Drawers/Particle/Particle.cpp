#include "Particle.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <numeric>

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
	color(std::numeric_limits<uint32_t>::max()),
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

	color = right.color;

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

	color = std::move(right.color);

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
}

void Particle::ThreadLoadTexture(const std::string& fileName) {
	tex = nullptr;
	TextureManager::GetInstance()->LoadTexture(fileName, &tex);
	isLoad = false;
}

void Particle::Update() {
	assert(wtfs.size() == wvpMat.Size());

	if (tex && tex->CanUse() && !isLoad) {
		srvHeap.InitializeReset(3);
		srvHeap.CreateTxtureView(tex);
		srvHeap.CreateStructuredBufferView(wvpMat);
		isLoad = true;
	}

	// currentSettingIndexがsettingの要素数を超えてたらassertで止める
	assert(currentSettingIndex < settings.size());
	auto nowTime = std::chrono::steady_clock::now();
	settings[currentSettingIndex].isValid.Update();

	// 有効になった瞬間始めた瞬間を保存
	if (settings[currentSettingIndex].isValid.OnEnter()) {
		settings[currentSettingIndex].startTime = nowTime;
		settings[currentSettingIndex].durationTime = nowTime;
		// パーティクルの最大数を保存
		this->Resize(settings[currentSettingIndex].emitter.particleMaxNum);
	}
	// 有効中
	else if (settings[currentSettingIndex].isValid.OnStay() ) {
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
				// ポジションランダム
				Vector3 maxPos = settings[currentSettingIndex].emitter.pos + settings[currentSettingIndex].emitter.size;
				Vector3 minPos = settings[currentSettingIndex].emitter.pos - settings[currentSettingIndex].emitter.size;
				[[maybe_unused]]Vector3 posRotate;
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
			
				// ステータスセット
				wtfs[i].pos = pos;
				wtfs[i].scale = size;
				wtfs[i].movePos = velocity;
				wtfs[i].deathTime = std::chrono::milliseconds(deathTime);
				wtfs[i].startTime = nowTime;
				wtfs[i].isActive = true;
			}

			// インデックスを更新
			currentParticleIndex = currentParticleIndex + particleNum;
		}
	}
}

void Particle::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend
) {
	if (tex && isLoad) {
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
			wvpMat[i] = viewProjection * VertMakeMatrixAffin(wtfs[i].scale, wtfs[i].rotate, wtfs[i].pos);
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
	ImGui::Begin(guiName.c_str());
	
	ImGui::DragFloat2("uvPibot", &uvPibot.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize.x, 0.01f);
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