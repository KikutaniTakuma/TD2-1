#include "Line.h"
#include "Engine/Engine.h"
#include <algorithm>
#include "Engine/PipelineManager/PipelineManager.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"

Shader Line::shader = {};

Pipeline* Line::pipline = nullptr;

void Line::Initialize() {
	shader.vertex = ShaderManager::LoadVertexShader("./Resources/Shaders/LineShader/Line.VS.hlsl");
	shader.pixel = ShaderManager::LoadPixelShader("./Resources/Shaders/LineShader/Line.PS.hlsl");

	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range.BaseShaderRegister = 0;
	range.NumDescriptors = 1;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER paramater = {};
	paramater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	paramater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	paramater.DescriptorTable.pDescriptorRanges = &range;
	paramater.DescriptorTable.NumDescriptorRanges = 1;

	PipelineManager::CreateRootSgnature(&paramater, 1, false);
	PipelineManager::SetVertexInput("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetVertexInput("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	PipelineManager::SetShader(shader);
	PipelineManager::SetState(Pipeline::None, Pipeline::SolidState::Solid, Pipeline::CullMode::None, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipline = PipelineManager::Create();
	PipelineManager::StateReset();
}

Line::Line() : 
	vertexBuffer(),
	vertexView{},
	vertexMap(nullptr),
	heap(),
	wvpMat(),
	start(),
	end()
{
	vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertexData) * kVertexNum);
	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * kVertexNum;
	vertexView.StrideInBytes = sizeof(VertexData);

	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap));

	heap = ShaderResourceHeap::GetInstance();

	heap->CreateConstBufferView(wvpMat);
}

Line::Line(const Line& right):
	Line()
{
	*this = right;
}
Line::Line(Line&& right) noexcept :
	Line()
{
	*this = std::move(right);
}

Line& Line::operator=(const Line& right) {
	start = right.start;
	end = right.end;

	return *this;
}
Line& Line::operator=(Line&& right)noexcept {
	start = std::move(right.start);
	end = std::move(right.end);

	return *this;
}

Line::~Line() {
	vertexBuffer->Release();
}

void Line::Draw(const Mat4x4& viewProjection, uint32_t color) {
	auto&& colorFloat = UintToVector4(color);
	vertexMap[0] = { Vector4(start, 1.0f), colorFloat };
	vertexMap[1] = { Vector4(end, 1.0f),   colorFloat };

	*wvpMat = viewProjection;

	pipline->Use();
	heap->Use(wvpMat.GetDescIndex(), 0);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &vertexView);
	Engine::GetCommandList()->DrawInstanced(kVertexNum, 1, 0, 0);
}