#include "ShaderResourceHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Engine.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numeric>

ShaderResourceHeap* ShaderResourceHeap::instance = nullptr;

void ShaderResourceHeap::Initialize(UINT numDescriptor) {
	// 1～(10^6-1)でクランプ
	numDescriptor = std::clamp(numDescriptor, 1u, static_cast<UINT>(std::pow(10u, 6u)) - 1u);

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
	heapHandles(0)/*,
	isUse(),
	releaseView()*/
{
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	heapHandles.reserve(heapSize);
	heapHandles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHandles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		hadleTmp.second.ptr += Engine::GetIncrementSRVCBVUAVHeap() * i;
		heapHandles.push_back(hadleTmp);
	}

	//isUse.resize(numDescriptor);
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
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHandles[handleIndex].second);
}

void ShaderResourceHeap::Reset() {
	if (SRVHeap) {
		SRVHeap->Release();
		SRVHeap.Reset();
	}
}

//void ShaderResourceHeap::SetReleasedIndexPos(UINT nextCreateViewNum) {
//	if (releaseView.empty() || nextCreateViewNum > releaseView.size()) {
//		return;
//	}
//}

uint32_t ShaderResourceHeap::CreateTxtureView(Texture* tex) {
	assert(tex != nullptr);
	if (tex == nullptr || !*tex) {
		return currentHadleIndex;
	}
	assert(currentHadleIndex < heapSize);
	if (currentHadleIndex >= heapSize /*|| isUse[currentHadleIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	tex->CreateSRVView(heapHandles[currentHadleIndex].first);

	//isUse[currentHadleIndex] = true;

	currentHadleIndex++;

	return currentHadleIndex - 1u;
}
void ShaderResourceHeap::CreateTxtureView(Texture* tex, uint32_t heapIndex) {
	assert(tex != nullptr);
	assert(heapIndex < heapSize);
	if (currentHadleIndex >= heapSize/* || isUse[heapIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return;
	}
	tex->CreateSRVView(heapHandles[heapIndex].first);

	//isUse[heapIndex] = true;
}

uint32_t ShaderResourceHeap::CreatePerarenderView(RenderTarget& renderTarget) {
	assert(currentHadleIndex < heapSize);
	if (currentHadleIndex >= heapSize/* || isUse[currentHadleIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatePerarenderView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	renderTarget.CreateView(heapHandles[currentHadleIndex].first, heapHandles[currentHadleIndex].second);
	currentHadleIndex++;

	//isUse[currentHadleIndex] = true;

	return currentHadleIndex - 1u;
}