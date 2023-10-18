#include "ShaderResourceHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include "Engine/WinApp/WinApp.h"
#include "Engine/Engine.h"

ShaderResourceHeap* ShaderResourceHeap::instance = nullptr;

void ShaderResourceHeap::Initialize(UINT numDescriptor) {
	instance = new ShaderResourceHeap{ numDescriptor };
}

void ShaderResourceHeap::Finalize() {
	delete instance;
	instance = nullptr;
}

ShaderResourceHeap* ShaderResourceHeap::GetInstance() {
	return instance;
}

ShaderResourceHeap::ShaderResourceHeap(UINT numDescriptor) :
	SRVHeap(),
	heapSize(numDescriptor),
#ifdef _DEBUG
	currentHadleIndex(1),
#else
	currentHadleIndex(0),
#endif // _DEBUG
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
	Reset();
}

void ShaderResourceHeap::SetHeap() {
	static auto commandlist = Engine::GetCommandList();
	commandlist->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
}

void ShaderResourceHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex) {
	static auto commandlist = Engine::GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, handle);
}

void ShaderResourceHeap::Use(uint32_t handleIndex, UINT rootParmIndex) {
	auto commandlist = Engine::GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHadles[handleIndex].second);
}

void ShaderResourceHeap::Reset() {
	if (SRVHeap) {
		SRVHeap->Release();
		SRVHeap.Reset();
	}
}