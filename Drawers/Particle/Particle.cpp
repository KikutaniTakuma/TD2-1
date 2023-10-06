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

Particle::Particle(uint32_t indexNum) :
	wtfs(),
	uvPibot(),
	uvSize(Vector2::identity),
	tex(nullptr),
	isLoad(false),
	color(std::numeric_limits<uint32_t>::max()),
	wvpMat(indexNum),
	colorBuf(),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f)
{
	for (uint32_t i = 0; i < wvpMat.Size();i++) {
		wvpMat[i] = MakeMatrixIndentity();
	}
	*colorBuf = Vector4::identity;

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
	*colorBuf = *right.colorBuf;

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
	*colorBuf = *right.colorBuf;

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
	range.NumDescriptors = 2;
	range.BaseShaderRegister = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_ROOT_PARAMETER, 2> rootPrams={};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = 1;
	rootPrams[0].DescriptorTable.pDescriptorRanges = &range;
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootPrams[1].Descriptor.ShaderRegister = 0;
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

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

	*colorBuf = UintToVector4(color);
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
		commandlist->SetGraphicsRootConstantBufferView(1, colorBuf.GetGPUVtlAdrs());
		commandlist->IASetVertexBuffers(0, 1, &vertexView);
		commandlist->IASetIndexBuffer(&indexView);
		commandlist->DrawIndexedInstanced(6, wvpMat.Size(), 0, 0, 0);
	}
}

void Particle::Debug(const std::string& guiName) {
	*colorBuf = UintToVector4(color);
	ImGui::Begin(guiName.c_str());
	
	ImGui::DragFloat2("uvPibot", &uvPibot.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf->color.r);
	color = Vector4ToUint(*colorBuf);
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