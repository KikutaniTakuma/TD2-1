#include "Texture2D.h"
#include "imgui.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include <numeric>
#undef max
#undef min
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

/// <summary>
/// 静的変数のインスタンス化
/// </summary>

std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum) * 2> Texture2D::graphicsPipelineState_ = {};
Shader Texture2D::shader_ = {};

D3D12_INDEX_BUFFER_VIEW Texture2D::indexView_ = {};
Microsoft::WRL::ComPtr<ID3D12Resource> Texture2D::indexResource_ = nullptr;

Texture2D::Texture2D() :
	scale_(Vector2::identity),
	rotate_(),
	pos_({ 0.0f,0.0f,0.01f }),
	uvPibot_(),
	uvSize_(Vector2::identity),
	tex_(nullptr),
	isLoad_(false),
	color_(std::numeric_limits<uint32_t>::max()),
	wvpMat_(),
	colorBuf_(),
	aniStartTime_(),
	aniCount_(0.0f),
	uvPibotSpd_(0.0f),
	isAnimation_(0.0f),
	isSameTexSize_(),
	texScalar_(1.0f)
{
	*wvpMat_ = Mat4x4::kIdentity_;
	*colorBuf_ = Vector4::identity;

	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
		vertexResource_ = nullptr;
	}

	vertexResource_ =DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexView_.SizeInBytes = sizeof(VertexData) * 4;
	vertexView_.StrideInBytes = sizeof(VertexData);

	tex_ = TextureManager::GetInstance()->GetWhiteTex();

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}

	auto srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(2u);
	srvHeap->CreateConstBufferView(wvpMat_);
	srvHeap->CreateConstBufferView(colorBuf_);
}

Texture2D::Texture2D(const std::string& fileName):
	Texture2D()
{
	this->LoadTexture(fileName);
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
	scale_ = right.scale_;
	rotate_ = right.rotate_;
	pos_ = right.pos_;

	uvPibot_ = right.uvPibot_;
	uvSize_ = right.uvSize_;

	color_ = right.color_;

	worldPos_ = right.worldPos_;

	tex_ = right.tex_;
	isLoad_ = right.isLoad_;


	*wvpMat_ = *right.wvpMat_;
	*colorBuf_ = *right.colorBuf_;

	aniStartTime_ = right.aniStartTime_;
	aniCount_ = right.aniCount_;
	isAnimation_ = right.isAnimation_;
	uvPibotSpd_ = right.uvPibotSpd_;

	isSameTexSize_ = right.isSameTexSize_;

	texScalar_ = right.texScalar_;

	return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& right) noexcept {
	scale_ = std::move(right.scale_);
	rotate_ = std::move(right.rotate_);
	pos_ = std::move(right.pos_);

	uvPibot_ = std::move(right.uvPibot_);
	uvSize_ = std::move(right.uvSize_);

	color_ = std::move(right.color_);

	worldPos_ = std::move(right.worldPos_);

	tex_ = std::move(right.tex_);
	isLoad_ = std::move(right.isLoad_);

	*wvpMat_ = *right.wvpMat_;
	*colorBuf_ = *right.colorBuf_;

	aniStartTime_ = std::move(right.aniStartTime_);
	aniCount_ = std::move(right.aniCount_);
	isAnimation_ = std::move(right.isAnimation_);
	uvPibotSpd_ = std::move(right.uvPibotSpd_);

	isSameTexSize_ = std::move(right.isSameTexSize_);
	texScalar_ = std::move(right.texScalar_);

	return *this;
}

Texture2D::~Texture2D() {
	auto descriptorHeap = CbvSrvUavHeap::GetInstance();
	descriptorHeap->ReleaseView(wvpMat_.GetViewHandleUINT());
	descriptorHeap->ReleaseView(colorBuf_.GetViewHandleUINT());

	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
		vertexResource_ = nullptr;
	}
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	if (indexResource_) { 
		indexResource_->Release();
		indexResource_.Reset();
		indexResource_ = nullptr;
	}
	
	LoadShader(vsFileName, psFileName);

	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(indices));
	indexView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexView_.SizeInBytes = sizeof(indices);
	indexView_.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource_->Unmap(0, nullptr);

	CreateGraphicsPipeline();
}

void Texture2D::Finalize() {
	if (indexResource_) { 
		indexResource_->Release();
		indexResource_.Reset();
		indexResource_ = nullptr;
	}
}

void Texture2D::LoadShader(const std::string& vsFileName, const std::string& psFileName) {
	shader_.vertex_ = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader_.vertex_);
	shader_.pixel_ = ShaderManager::LoadPixelShader(psFileName);
	assert(shader_.pixel_);
}

void Texture2D::CreateGraphicsPipeline() {
	std::array<D3D12_DESCRIPTOR_RANGE, 1> texRange = {};
	texRange[0].NumDescriptors = 1;
	texRange[0].BaseShaderRegister = 0;
	texRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	texRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].NumDescriptors = 2;
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_APPEND_ALIGNED_ELEMENT;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	std::array<D3D12_ROOT_PARAMETER, 2> rootPrams{};
	rootPrams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[0].DescriptorTable.NumDescriptorRanges = UINT(texRange.size());
	rootPrams[0].DescriptorTable.pDescriptorRanges = texRange.data();
	rootPrams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootPrams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootPrams[1].DescriptorTable.NumDescriptorRanges = UINT(cbvRange.size());
	rootPrams[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	rootPrams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


	PipelineManager::CreateRootSgnature(rootPrams.data(), rootPrams.size(), true);
	PipelineManager::SetShader(shader_);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	for (int32_t i = Pipeline::Blend::None; i < graphicsPipelineState_.size(); i++) {
		if (i < Pipeline::Blend::BlendTypeNum) {
			PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
			graphicsPipelineState_[i] = PipelineManager::Create();
		}
		else {
			PipelineManager::IsDepth(false);
			PipelineManager::SetState(Pipeline::Blend(i - Pipeline::Blend::BlendTypeNum), Pipeline::SolidState::Solid);
			graphicsPipelineState_[i] = PipelineManager::Create();
		}
	}

	PipelineManager::StateReset();

	for (auto& i : graphicsPipelineState_) {
		if (!i) {
			Log::ErrorLog("pipeline is nullptr", "CreateGraphicsPipeline()", "Texture2D");
			return;
		}
	}
}

void Texture2D::LoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	while (textureManager->IsNowThreadLoading()) {
		// 非同期読み込みが終わるまでビジーループ
	}
	tex_ = textureManager->LoadTexture(fileName);

	if (tex_ && !isLoad_) {
		isLoad_ = true;
	}
}

void Texture2D::ThreadLoadTexture(const std::string& fileName) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	tex_ = nullptr;
	textureManager->LoadTexture(fileName, &tex_);
	isLoad_ = false;
}

void Texture2D::Update() {

	if (tex_ && tex_->CanUse() && !isLoad_) {
		isLoad_ = true;
	}

	if (tex_ && isLoad_) {
		if (isSameTexSize_) {
			scale_ = tex_->getSize() * texScalar_;
		}
		else if(isSameTexSize_.OnExit()) {
			scale_.x /= tex_->getSize().x;
			scale_.y /= tex_->getSize().y;
		}

		static const std::array<Vector3, 4> pv{
			Vector3{ -0.5f,  0.5f, 0.1f },
			Vector3{  0.5f,  0.5f, 0.1f },
			Vector3{  0.5f, -0.5f, 0.1f },
			Vector3{ -0.5f, -0.5f, 0.1f },
		};

		std::copy(pv.begin(), pv.end(), worldPos_.begin());
		auto&& worldMat =
			MakeMatrixAffin(
				Vector3(scale_.x, scale_.y, 1.0f),
				rotate_,
				pos_
			);
		for (auto& i : worldPos_) {
			i *= worldMat;
		}

		*colorBuf_ = UintToVector4(color_);
	}
	isSameTexSize_.Update();
}

void Texture2D::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend,
	bool isDepth
) {
	if (tex_ && isLoad_) {
		const Vector2& uv0 = { uvPibot_.x, uvPibot_.y + uvSize_.y }; const Vector2& uv1 = uvSize_ + uvPibot_;
		const Vector2& uv2 = { uvPibot_.x + uvSize_.x, uvPibot_.y }; const Vector2& uv3 = uvPibot_;

		std::array<VertexData, 4> pv = {
			worldPos_[0], uv3,
			worldPos_[1], uv2,
			worldPos_[2], uv1,
			worldPos_[3], uv0,
		};

		VertexData* mappedData = nullptr;
		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
		std::copy(pv.begin(), pv.end(), mappedData);
		vertexResource_->Unmap(0, nullptr);

		*wvpMat_ = viewProjection;

		auto commandlist = DirectXCommand::GetInstance()->GetCommandList();


		// 各種描画コマンドを積む
		if (isDepth) {
			graphicsPipelineState_[blend]->Use();
		}
		else {
			graphicsPipelineState_[blend + Pipeline::Blend::BlendTypeNum]->Use();
		}
		
		tex_->Use(0);
		commandlist->SetGraphicsRootDescriptorTable(1, wvpMat_.GetViewHandle());
		commandlist->IASetVertexBuffers(0, 1, &vertexView_);
		commandlist->IASetIndexBuffer(&indexView_);
		commandlist->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture2D::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	*colorBuf_ = UintToVector4(color_);
	ImGui::Begin(guiName.c_str());
	ImGui::Checkbox("is same scale and Texture", isSameTexSize_.Data());
	if (isSameTexSize_) {
		ImGui::DragFloat("tex scalar", &texScalar_, 0.01f);
	}
	ImGui::DragFloat2("scale", &scale_.x, 1.0f);
	ImGui::DragFloat3("rotate", &rotate_.x, 0.01f);
	ImGui::DragFloat3("pos", &pos_.x, 1.0f);
	ImGui::DragFloat2("uvPibot", &uvPibot_.x, 0.01f);
	ImGui::DragFloat2("uvSize", &uvSize_.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf_->color.r);
	color_ = Vector4ToUint(*colorBuf_);

	if (ImGui::TreeNode("tex load")) {
		if (isLoad_) {
		auto texures = Lamb::GetFilePathFormDir("./Resources/", ".png");

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

bool Texture2D::Collision(const Vector2& pos2D) const {
	Vector2 max;
	Vector2 min;
	max.x = std::max_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.x = std::min_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos_.begin(), worldPos_.end(),
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

bool Texture2D::Collision(const Texture2D& tex2D) const {
	Vector3 max;
	Vector3 min;
	max.x = std::max_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max.y = std::max_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max.z = std::max_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min.x = std::min_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min.y = std::min_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min.z = std::min_element(worldPos_.begin(), worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;

	// 追加変更。by Korone
	Vector3 max2;
	Vector3 min2;
	max2.x = std::max_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	max2.y = std::max_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	max2.z = std::max_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;
	min2.x = std::min_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.x < right.x;
		})->x;
	min2.y = std::min_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.y < right.y;
		})->y;
	min2.z = std::min_element(tex2D.worldPos_.begin(), tex2D.worldPos_.end(),
		[](const Vector3& left, const Vector3& right) {
			return left.z < right.z;
		})->z;

	if (min.x <= max2.x && max.x >= min2.x &&
		min.y <= max2.y && max.y >= min2.y) {
		return true;
	}

	/*for (auto& i : tex2D.worldPos) {
		if (min.x <= i.x && i.x <= max.x) {
			if (min.y <= i.y && i.y <= max.y) {
				return true;
			}
		}
	}*/

	return false;
}

void Texture2D::ChangeTexture(Texture* tex) {
	tex_ = tex;
	isLoad_ = !!tex_;
}

void Texture2D::AnimationStart(float aniUvPibot) {
	if (!isAnimation_) {
		aniStartTime_ = std::chrono::steady_clock::now();
		isAnimation_ = true;
		aniCount_ = 0.0f;
		uvPibot_.x = aniUvPibot;
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

			uvPibot_.x = aniUvStart;
			uvPibot_.x += uvPibotSpd_ * aniCount_;
		}
	}
}