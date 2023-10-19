#include "PipelineManager.h"
#include <cassert>
#include <algorithm>
#include "Engine/RootSignature/RootSignature.h"

PipelineManager* PipelineManager::instance = nullptr;

void PipelineManager::Initialize() {
	instance = new PipelineManager();
	assert(instance);
}
void PipelineManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void PipelineManager::CreateRootSgnature(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_) {
	auto rootSignature = std::make_unique<RootSignature>();

	rootSignature->Create(rootParamater_, rootParamaterSize_, isTexture_);

	instance->rootSignature = rootSignature.get();

	instance->rootSignatures.push_back(std::move(rootSignature));
}
void PipelineManager::SetRootSgnature(RootSignature* rootSignature) {
	instance->rootSignature = rootSignature;
}

void PipelineManager::SetVertexInput(std::string semanticName_, uint32_t semanticIndex_, DXGI_FORMAT format_) {
	instance->vertexInputStates.push_back({ semanticName_, semanticIndex_, format_ });
}
void PipelineManager::SetShader(const Shader& shader_) {
	instance->shader = shader_;
}

void PipelineManager::SetState(
	Pipeline::Blend blend_,
	Pipeline::SolidState solidState_,
	Pipeline::CullMode cullMode_,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_,
	uint32_t numRenderTarget_
) {
	instance->blend = std::clamp(blend_, Pipeline::Blend::None, Pipeline::Blend(Pipeline::Blend::BlendTypeNum - 1));
	instance->cullMode = cullMode_;
	instance->solidState = solidState_;
	instance->topologyType = topologyType_;
	instance->numRenderTarget = numRenderTarget_;
}

void PipelineManager::IsDepth(bool isDepth_) {
	instance->isDepth = isDepth_;
}

Pipeline* PipelineManager::Create() {
	auto pipeline = std::make_unique<Pipeline>();
	pipeline->SetShader(instance->shader);
	for (auto& i : instance->vertexInputStates) {
		pipeline->SetVertexInput(std::get<std::string>(i), std::get<uint32_t>(i), std::get<DXGI_FORMAT>(i));
	}
	pipeline->Create(
		*instance->rootSignature,
		instance->blend,
		instance->cullMode,
		instance->solidState,
		instance->topologyType,
		instance->numRenderTarget,
		instance->isDepth
	);

	if (!pipeline->graphicsPipelineState) {
		return nullptr;
	}

	instance->pipelines.push_back(std::move(pipeline));

	return instance->pipelines.back().get();
}

void PipelineManager::StateReset() {
	instance->rootSignature = nullptr;
	instance->shader = { nullptr };
	instance->vertexInputStates.clear();
	instance->blend = {};
	instance->cullMode = {};
	instance->solidState = {};
	instance->topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	instance->numRenderTarget = 0u;
	instance->isDepth = true;
}

PipelineManager::PipelineManager() :
	pipelines(),
	rootSignatures(),
	rootSignature(nullptr),
	shader{},
	blend(),
	cullMode(),
	solidState(),
	topologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE),
	numRenderTarget(0u),
	vertexInputStates(0),
	isDepth(true)
{}