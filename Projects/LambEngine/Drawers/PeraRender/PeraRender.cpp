#include "PeraRender.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "imgui.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include <cassert>
#include <numbers>

PeraRender::PeraRender():
	render_(),
	peraVertexResource_(nullptr),
	peraVertexView_(),
	indexView_{},
	indexResource_{nullptr},
	shader_{},
	piplines_{nullptr},
	pos_{},
	rotate_{},
	scale_{Vector3::identity},
	wvpMat_{},
	colorBuf_{},
	isPreDraw_(false),
	uvPibot_(),
	uvSize_(Vector2::identity),
	worldPos_{},
	color_(std::numeric_limits<uint32_t>::max())
{}

PeraRender::PeraRender(uint32_t width, uint32_t height):
	render_(width, height),
	peraVertexResource_(nullptr),
	peraVertexView_(),
	indexView_{},
	indexResource_{ nullptr },
	shader_{},
	piplines_{ nullptr },
	pos_{},
	rotate_{},
	scale_{ Vector3::identity },
	wvpMat_{},
	colorBuf_{},
	isPreDraw_(false),
	uvPibot_(),
	uvSize_(Vector2::identity),
	worldPos_{},
	color_(std::numeric_limits<uint32_t>::max())
{}

PeraRender::~PeraRender() {
	static auto srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->ReleaseView(render_.GetViewHandleUINT());
	if (peraVertexResource_) {
		peraVertexResource_->Release();
		peraVertexResource_.Reset();
	}
	if (indexResource_) {
		indexResource_->Release();
		indexResource_.Reset();
	}
}

void PeraRender::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateShader(vsFileName, psFileName);

	CreateGraphicsPipeline();

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

	std::array<PeraVertexData, 4> pv = {
		Vector3{ -0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 1.0f },
		Vector3{  0.5f,  0.5f, 0.1f }, Vector2{ 0.0f, 0.0f },
		Vector3{  0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 1.0f },
		Vector3{ -0.5f, -0.5f, 0.1f }, Vector2{ 1.0f, 0.0f }
	};

	peraVertexResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(pv));

	peraVertexView_.BufferLocation = peraVertexResource_->GetGPUVirtualAddress();
	peraVertexView_.SizeInBytes = sizeof(pv);
	peraVertexView_.StrideInBytes = sizeof(PeraVertexData);

	PeraVertexData* mappedData = nullptr;
	peraVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource_->Unmap(0, nullptr);

	static auto srvHeap = CbvSrvUavHeap::GetInstance();
	srvHeap->BookingHeapPos(3u);
	srvHeap->CreatePerarenderView(render_);
	srvHeap->CreateConstBufferView(wvpMat_);
	srvHeap->CreateConstBufferView(colorBuf_);
}

void PeraRender::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	shader_.vertex_ = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader_.vertex_);
	shader_.pixel_ = ShaderManager::LoadPixelShader(psFileName);
	assert(shader_.pixel_);
}

void PeraRender::CreateGraphicsPipeline() {
	std::array<D3D12_DESCRIPTOR_RANGE, 1> renderRange = {};
	renderRange[0].BaseShaderRegister = 0;
	renderRange[0].NumDescriptors = 1;
	renderRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	renderRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	std::array<D3D12_DESCRIPTOR_RANGE, 1> cbvRange = {};
	cbvRange[0].BaseShaderRegister = 0;
	cbvRange[0].NumDescriptors = 1;
	cbvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	std::array<D3D12_ROOT_PARAMETER, 2> roootParamater = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = renderRange.data();
	roootParamater[0].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(renderRange.size());
	
	roootParamater[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater[1].DescriptorTable.pDescriptorRanges = cbvRange.data();
	roootParamater[1].DescriptorTable.NumDescriptorRanges = static_cast<UINT>(cbvRange.size());

	PipelineManager::CreateRootSgnature(roootParamater.data(), roootParamater.size(), true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader_);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	PipelineManager::IsDepth(false);

	for (int32_t i = Pipeline::Blend::None; i < Pipeline::Blend::BlendTypeNum; i++) {
		PipelineManager::SetState(Pipeline::Blend(i), Pipeline::SolidState::Solid);
		piplines_[i] = PipelineManager::Create();
	}

	PipelineManager::StateReset();
}

void PeraRender::Update() {
	isPreDraw_ = false;

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

void PeraRender::PreDraw() {
	render_.SetThisRenderTarget();
	isPreDraw_ = true;
}

void PeraRender::Draw(
	const Mat4x4& viewProjection,
	Pipeline::Blend blend, 
	PeraRender* pera
) {
	if (!!pera) {
		pera->PreDraw();
		render_.ChangeResourceState();
	}
	else {
		// 描画先をメインレンダーターゲットに変更
		render_.SetMainRenderTarget();
	}

	const Vector2& uv0 = { uvPibot_.x, uvPibot_.y + uvSize_.y }; const Vector2& uv1 = uvSize_ + uvPibot_;
	const Vector2& uv2 = { uvPibot_.x + uvSize_.x, uvPibot_.y }; const Vector2& uv3 = uvPibot_;

	std::array<PeraVertexData, 4> pv = {
		worldPos_[0], uv3,
		worldPos_[1], uv2,
		worldPos_[2], uv1,
		worldPos_[3], uv0,
	};

	PeraVertexData* mappedData = nullptr;
	peraVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource_->Unmap(0, nullptr);

	*wvpMat_ = viewProjection;

	// 各種描画コマンドを積む
	ID3D12GraphicsCommandList* commandList = DirectXCommand::GetInstance()->GetCommandList();
	piplines_[blend]->Use();
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	render_.UseThisRenderTargetShaderResource();
	commandList->SetGraphicsRootDescriptorTable(1, wvpMat_.GetViewHandle());
	commandList->IASetVertexBuffers(0, 1, &peraVertexView_);
	commandList->IASetIndexBuffer(&indexView_);
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}