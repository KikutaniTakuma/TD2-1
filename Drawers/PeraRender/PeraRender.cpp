#include "PeraRender.h"
#include "Engine/Engine.h"
#include <cassert>
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
#include "externals/imgui/imgui.h"

PeraRender::PeraRender():
	render(),
	peraVertexResource(nullptr),
	peraVertexView(),
	shader{},
	piplines{nullptr, nullptr}
{}

PeraRender::PeraRender(uint32_t width_, uint32_t height_):
	render(width_, height_),
	peraVertexResource(nullptr),
	peraVertexView(),
	shader{},
	piplines{ nullptr, nullptr } 
{}

PeraRender::~PeraRender() {
	static auto srvHeap = ShaderResourceHeap::GetInstance();
	srvHeap->ReleaseView(render.GetViewHandleUINT());
	if (peraVertexResource) {
		peraVertexResource->Release();
		peraVertexResource.Reset();
	}
}

void PeraRender::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateShader(vsFileName, psFileName);

	CreateGraphicsPipeline();

	std::array<PeraVertexData, 4> pv = {
		Vector3{ -1.0f,-1.0f, 0.1f }, Vector2{ 0.0f, 1.0f },
		Vector3{ -1.0f, 1.0f, 0.1f }, Vector2{ 0.0f, 0.0f },
		Vector3{  1.0f,-1.0f, 0.1f }, Vector2{ 1.0f, 1.0f },
		Vector3{  1.0f, 1.0f, 0.1f }, Vector2{ 1.0f, 0.0f }
	};

	peraVertexResource = Engine::CreateBufferResuorce(sizeof(pv));

	peraVertexView.BufferLocation = peraVertexResource->GetGPUVirtualAddress();
	peraVertexView.SizeInBytes = sizeof(pv);
	peraVertexView.StrideInBytes = sizeof(PeraVertexData);

	PeraVertexData* mappedData = nullptr;
	peraVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	std::copy(pv.begin(), pv.end(), mappedData);
	peraVertexResource->Unmap(0, nullptr);

	static auto srvHeap = ShaderResourceHeap::GetInstance();
	srvHeap->BookingHeapPos(1u);
	srvHeap->CreatePerarenderView(render);
}

void PeraRender::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	shader.vertex = ShaderManager::LoadVertexShader(vsFileName);
	assert(shader.vertex);
	shader.pixel = ShaderManager::LoadPixelShader(psFileName);
	assert(shader.pixel);
}

void PeraRender::CreateGraphicsPipeline() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER roootParamater[1] = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	roootParamater[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	
	PipelineManager::CreateRootSgnature(roootParamater, 1, true);

	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	PipelineManager::SetVertexInput("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	PipelineManager::SetShader(shader);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid);

	PipelineManager::IsDepth(false);

	piplines[0] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Normal, Pipeline::SolidState::Solid);
	piplines[1] = PipelineManager::Create();

	PipelineManager::SetState(Pipeline::Add, Pipeline::SolidState::Solid);
	piplines[2] = PipelineManager::Create();

	PipelineManager::StateReset();
}

void PeraRender::PreDraw() {
	render.SetThisRenderTarget();
}

void PeraRender::Draw(Pipeline::Blend blend, PeraRender* pera) {
	if (!!pera) {
		pera->PreDraw();
		render.ChangeResourceState();
	}
	else {
		// 描画先をメインレンダーターゲットに変更
		render.SetMainRenderTarget();
	}

	// 各種描画コマンドを積む
	switch (blend){
	case Pipeline::None:
	case Pipeline::Sub:
	case Pipeline::Mul:
	case Pipeline::BlendTypeNum:
	default:
		piplines[0]->Use();
		break;

	case Pipeline::Normal:
		piplines[1]->Use();
		break;

	case Pipeline::Add:
		piplines[2]->Use();
		break;
	}
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &peraVertexView);
	render.UseThisRenderTargetShaderResource();
	Engine::GetCommandList()->DrawInstanced(4, 1, 0, 0);
}