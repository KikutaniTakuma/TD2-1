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
	if (instance->rootSignatures.empty()) {
		auto rootSignature = std::make_unique<RootSignature>();

		rootSignature->Create(rootParamater_, rootParamaterSize_, isTexture_);

		instance->rootSignature = rootSignature.get();

		instance->rootSignatures.push_back(std::move(rootSignature));
	}
	else {
		auto IsSame = [&rootParamater_,&rootParamaterSize_, &isTexture_](const std::unique_ptr<RootSignature>& rootSignature_) {
			return rootSignature_->IsSame(rootParamater_, rootParamaterSize_, isTexture_);
		};

		auto rootSignatureItr = std::find_if(instance->rootSignatures.begin(), instance->rootSignatures.end(), IsSame);

		if (rootSignatureItr == instance->rootSignatures.end()) {
			auto rootSignature = std::make_unique<RootSignature>();

			rootSignature->Create(rootParamater_, rootParamaterSize_, isTexture_);

			instance->rootSignature = rootSignature.get();

			instance->rootSignatures.push_back(std::move(rootSignature));
		}
		else {
			instance->rootSignature = rootSignatureItr->get();
		}
	}
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
	if (instance->pipelines.empty()) {
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

		return instance->pipelines.rbegin()->get();
	}
	else {
		auto IsSmae = [](const std::unique_ptr<Pipeline>& pipeline) {
			bool issame = pipeline->IsSame(
				instance->shader,
				instance->blend,
				instance->cullMode,
				instance->solidState,
				instance->topologyType,
				instance->numRenderTarget,
				instance->rootSignature->Get(),
				instance->isDepth
			);

			return issame;
		};

		auto pipelineItr = std::find_if(instance->pipelines.begin(), instance->pipelines.end(),IsSmae);

		if (pipelineItr == instance->pipelines.end()) {
			auto pipeline = std::make_unique<Pipeline>();
			pipeline->SetShader(instance->shader);
			for (auto& i : instance->vertexInputStates) {
				pipeline->SetVertexInput(std::get<0>(i), std::get<1>(i), std::get<2>(i));
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

			return instance->pipelines.rbegin()->get();
		}
		else {
			return pipelineItr->get();
		}
	}
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