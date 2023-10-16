#include "Texture2D.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <numeric>

/// <summary>
/// 静的変数のインスタンス化
/// </summary>

std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> Texture2D::graphicsPipelineState = {};
Shader Texture2D::shader = {};

D3D12_INDEX_BUFFER_VIEW Texture2D::indexView = {};
Microsoft::WRL::ComPtr<ID3D12Resource> Texture2D::indexResource = nullptr;

Texture2D::Texture2D() :
	scale(Vector2::identity),
	rotate(),
	pos({ 0.0f,0.0f,0.01f }),
	uvPibot(),
	uvSize(Vector2::identity),
	tex(nullptr),
	isLoad(false),
	color(std::numeric_limits<uint32_t>::max()),
	wvpMat(),
	colorBuf(),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f),
	isSameTexSize()
{
	*wvpMat = MakeMatrixIndentity();
	*colorBuf = Vector4::identity;

	if (vertexResource) { vertexResource->Release(); }

	vertexResource = Engine::CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 4;
	vertexView.StrideInBytes = sizeof(VertexData);

	tex = TextureManager::GetInstance()->GetWhiteTex();

	if (tex && !isLoad) {
		isLoad = true;
	}
}

Texture2D::Texture2D(const Texture2D& right) :
	Texture2D()
{
	*this = right;
}
Texture2D::Texture2D(Texture2D&& right) noexcept :
	Texture2D()
{
	*this = std::move(right);
}

Texture2D& Texture2D::operator=(const Texture2D& right) {
	scale = right.scale;
	rotate = right.rotate;
	pos = right.pos;

	uvPibot = right.uvPibot;
	uvSize = right.uvSize;

	color = right.color;

	worldPos = right.worldPos;

	tex = right.tex;
	isLoad = right.isLoad;


	*wvpMat = *right.wvpMat;
	*colorBuf = *right.colorBuf;

	aniStartTime_ = right.aniStartTime_;
	aniCount_ = right.aniCount_;
	isAnimation_ = right.isAnimation_;
	uvPibotSpd_ = right.uvPibotSpd_;

	isSameTexSize = right.isSameTexSize;

	return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& right) noexcept {
	scale = std::move(right.scale);
	rotate = std::move(right.rotate);
	pos = std::move(right.pos);

	uvPibot = std::move(right.uvPibot);
	uvSize = std::move(right.uvSize);

	color = std::move(right.color);

	worldPos = std::move(right.worldPos);

	tex = std::move(right.tex);
	isLoad = std::move(right.isLoad);

	*wvpMat = *right.wvpMat;
	*colorBuf = *right.colorBuf;

	aniStartTime_ = std::move(right.aniStartTime_);
	aniCount_ = std::move(right.aniCount_);
	isAnimation_ = std::move(right.isAnimation_);
	uvPibotSpd_ = std::move(right.uvPibotSpd_);

	isSameTexSize = std::move(right.isSameTexSize);

	return *this;
}

Texture2D::~Texture2D() {
	if (vertexResource) {
		vertexResource->Release();
		vertexResource.Reset();
	}
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	if (indexResource) { 
		indexResource->Release();
		indexResource.Reset();
	}
	
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

void Texture2D::Finalize() {
	if (indexResource) { 
		indexResource->Release(); 
		indexResource.Reset();
	}
}

void Texture2D::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void Texture2D::CreateGraphicsPipeline() {
	D3D12_DESCRIPTOR_RANGE range = {};
	range.NumDescriptors = 1;
	range.BaseShaderRegister = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_ROOT_PARAMETER, 3> rootPrams{};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootPrams[0].Descriptor.ShaderRegister = 0;
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootPrams[1].Descriptor.ShaderRegister = 1;
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootPrams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[2].DescriptorTable.NumDescriptorRanges = 1;
	rootPrams[2].DescriptorTable.pDescriptorRanges = &range;
	rootPrams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true);
	PipelineManager::SetShader(shader);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	//PipelineManager::IsDepth(false);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		graphicsPipelineState[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();

	for (auto& i : graphicsPipelineState) {
		if (!i) {
			ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Texture2D");
			return;
		}
	}
}

void Texture2D::LoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	while (true) {
		if (textureManager->ThreadLoadFinish()) {
			tex = textureManager->LoadTexture(fileName);
			break;
		}
	}

	if (tex && !isLoad) {
		isLoad = true;
	}
}

void Texture2D::ThreadLoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	tex = nullptr;
	textureManager->LoadTexture(fileName, &tex);
	isLoad = false;
}

void Texture2D::Update() {

	if (tex && tex->CanUse() && !isLoad) {
		isLoad = true;
	}

	if (tex && isLoad) {
		if (isSameTexSize) {
			scale = tex->getSize();
		}
		else if(isSameTexSize.OnExit()) {
			scale.x /= tex->getSize().x;
			scale.y /= tex->getSize().y;
		}

		static const std::array<Vector3, 4> pv{
			Vector3{ -0.5f,  0.5f, 0.1f },
			Vector3{  0.5f,  0.5f, 0.1f },
			Vector3{  0.5f, -0.5f, 0.1f },
			Vector3{ -0.5f, -0.5f, 0.1f },
		};

		std::copy(pv.begin(), pv.end(), worldPos.begin());
		auto&& worldMat =
			HoriMakeMatrixAffin(
				Vector3(scale.x, scale.y, 1.0f),
				rotate,
				pos
			);
		for (auto& i : worldPos) {
			i *= worldMat;
		}

		*colorBuf = UintToVector4(color);
	}
	isSameTexSize.Update();
}

void Texture2D::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend
) {
	if (tex && isLoad) {
		const Vector2& uv0 = { uvPibot.x, uvPibot.y + uvSize.y }; const Vector2& uv1 = uvSize + uvPibot;
		const Vector2& uv2 = { uvPibot.x + uvSize.x, uvPibot.y }; const Vector2& uv3 = uvPibot;

		std::array<VertexData, 4> pv = {
			worldPos[0], uv3,
			worldPos[1], uv2,
			worldPos[2], uv1,
			worldPos[3], uv0,
		};

		VertexData* mappedData = nullptr;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource->Unmap(0, nullptr);

		*wvpMat = viewProjection;

		auto commandlist = Engine::GetCommandList();

		// 各種描画コマンドを積む
		graphicsPipelineState[blend]->Use();
		commandlist->SetGraphicsRootConstantBufferView(0, wvpMat.GetGPUVtlAdrs());
		commandlist->SetGraphicsRootConstantBufferView(1, colorBuf.GetGPUVtlAdrs());
		tex->Use(2);
		commandlist->IASetVertexBuffers(0, 1, &vertexView);
		commandlist->IASetIndexBuffer(&indexView);
		commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture2D::Debug(const std::string& guiName) {
#ifdef _DEBUG


	*colorBuf = UintToVector4(color);
	ImGui::Begin(guiName.c_str());
	ImGui::Checkbox("is same scale and Texture", isSameTexSize.Data());
	ImGui::DragFloat2("scale", &scale.x, 1.0f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("pos", &pos.x, 1.0f);
	ImGui::DragFloat2("uvPibot", &uvPibot.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf->color.r);
	color = Vector4ToUint(*colorBuf);

	if (ImGui::TreeNode("tex load")) {
		if (isLoad) {
		auto texures = UtilsLib::GetFilePahtFormDir("./Resources/", ".png");

			for (auto& i : texures) {
				if (ImGui::Button(i.string().c_str())) {
					this->ThreadLoadTexture(i.string());
					break;
				}
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();
#endif // _DEBUG
}

bool Texture2D::Colision(const Vector2& pos2D) const {
	Vector2 max;
	Vector2 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;

	if (min.x < pos2D.x && pos2D.x < max.x) {
		if (min.y < pos2D.y && pos2D.y < max.y) {
			return true;
		}
	}

	return false;
}

bool Texture2D::Colision(const Texture2D& tex2D) const {
	Vector3 max;
	Vector3 min;
	max.x = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max.z = std::max_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min.x = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.z = std::min_element(worldPos.begin(), worldPos.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;


	for (auto& i : tex2D.worldPos) {
		if (min.x < i.x && i.x < max.x) {
			if (min.y < i.y && i.y < max.y) {
				if (min.z < i.z && i.z < max.z) {
					return true;
				}
			}
		}
	}

	return false;
}

void Texture2D::AnimationStart(float aniUvPibot) {
	if (!isAnimation_) {
		aniStartTime_ = std::chrono::steady_clock::now();
		isAnimation_ = true;
		aniCount_ = 0.0f;
		uvPibot.x = aniUvPibot;
	}
}

void Texture2D::AnimationPause() {
	isAnimation_ = false;
}

void Texture2D::AnimationRestart() {
	isAnimation_ = true;
}

void Texture2D::Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd) {
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