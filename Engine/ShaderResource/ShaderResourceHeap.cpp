#include "ShaderResourceHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>

ShaderResourceHeap::ShaderResourceHeap() :
	SRVHeap(),
	heapOrder(0),
	descriptorRanges(0),
	heapSize(4),
	currentHadleIndex(),
	heapHadles(0)
{
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize, true);

	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(), 
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}
}

ShaderResourceHeap::ShaderResourceHeap(const ShaderResourceHeap& right)
{
	*this = right;
}

ShaderResourceHeap::ShaderResourceHeap(ShaderResourceHeap&& right) noexcept {
	*this = std::move(right);
}

ShaderResourceHeap::ShaderResourceHeap(uint16_t numDescriptor) :
	SRVHeap(),
	heapOrder(0),
	descriptorRanges(0),
	heapSize(numDescriptor),
	currentHadleIndex(0),
	heapHadles(0)
{
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}
}

ShaderResourceHeap::~ShaderResourceHeap() {
	if (SRVHeap) {
		SRVHeap->Release();
		SRVHeap.Reset();
	}
}

ShaderResourceHeap& ShaderResourceHeap::operator=(const ShaderResourceHeap& right) {
	SRVHeap = right.SRVHeap;

	heapOrder = right.heapOrder;
	descriptorRanges = right.descriptorRanges;

	heapSize = right.heapSize;
	currentHadleIndex = right.currentHadleIndex;

	heapHadles.clear();
	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}

	return *this;
}

ShaderResourceHeap& ShaderResourceHeap::operator=(ShaderResourceHeap&& right) noexcept {
	SRVHeap = std::move(right.SRVHeap);

	heapOrder = std::move(right.heapOrder);
	descriptorRanges = std::move(right.descriptorRanges);

	heapSize = std::move(right.heapSize);
	currentHadleIndex = std::move(right.currentHadleIndex);

	heapHadles.clear();
	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}

	return *this;
}

void ShaderResourceHeap::InitializeReset() {
	SRVHeap->Release();
	SRVHeap.Reset();
	heapOrder.clear();
	descriptorRanges.clear();
	heapHadles.clear();
	heapSize = 4;
	currentHadleIndex = 0;

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize, true);

	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}
}

void ShaderResourceHeap::InitializeReset(uint32_t numDescriptor) {
	SRVHeap->Release();
	SRVHeap.Reset();
	heapOrder.clear();
	descriptorRanges.clear();
	heapHadles.clear();
	heapSize = numDescriptor;
	currentHadleIndex = 0;

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize, true);

	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}
}

void ShaderResourceHeap::Use() {
	auto commandlist = Engine::GetCommandList();
	commandlist->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
	auto SrvHandle = SRVHeap->GetGPUDescriptorHandleForHeapStart();
	commandlist->SetGraphicsRootDescriptorTable(0, SrvHandle);
}

void ShaderResourceHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle) {
	auto commandlist = Engine::GetCommandList();
	commandlist->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
	commandlist->SetGraphicsRootDescriptorTable(0, handle);
}

D3D12_ROOT_PARAMETER ShaderResourceHeap::GetParameter() {
	uint32_t descriptorNum = 1u;

	auto nextHeapType = heapOrder.begin();
	if (heapOrder.size() != 1) {
		nextHeapType++;
	}
	uint32_t shaderRegisterCountSRV = 0u;
	uint32_t shaderRegisterCountCBV = 0u;
	uint32_t shaderRegisterCountUAV = 0u;
	for (auto itr = heapOrder.begin(); itr != heapOrder.end();) {
		if (heapOrder.size() == 1 || nextHeapType == heapOrder.end() || *nextHeapType != *itr) {
			D3D12_DESCRIPTOR_RANGE descriptorRange{};

			descriptorRange.NumDescriptors = descriptorNum;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			switch (*itr)
			{
			case HeapType::CBV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountCBV;
				shaderRegisterCountCBV += descriptorNum;
				break;

			case HeapType::SRV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountSRV;
				shaderRegisterCountSRV += descriptorNum;
				break;

			case HeapType::UAV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountUAV;
				shaderRegisterCountUAV += descriptorNum;
				break;
			}

			descriptorRanges.push_back(descriptorRange);
			descriptorNum = 0u;

			if (heapOrder.size() == 1 || nextHeapType == heapOrder.end()) {
				break;
			}
		}

		nextHeapType = ++itr;
		nextHeapType++;
		descriptorNum++;
	}

	D3D12_ROOT_PARAMETER roootParamater{};
	roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater.DescriptorTable.pDescriptorRanges = descriptorRanges.data();
	roootParamater.DescriptorTable.NumDescriptorRanges = UINT(descriptorRanges.size());

	return roootParamater;
}

void ShaderResourceHeap::Reset() {
	if (SRVHeap) {
		SRVHeap->Release();
		SRVHeap.Reset();
	}

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize, true);

	heapHadles.clear();
	heapHadles.reserve(heapSize);
	heapHadles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHadles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHadles.push_back(hadleTmp);
	}

	currentHadleIndex = 0;
	heapOrder.clear();
	descriptorRanges.clear();
}