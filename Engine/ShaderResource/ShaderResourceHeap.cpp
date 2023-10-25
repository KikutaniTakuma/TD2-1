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
	currentHandleIndex(1),
#else
	currentHandleIndex(0),
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

	isUseHandle_.resize(numDescriptor);
	for (size_t i = 0; i < isUseHandle_.size(); i++) {
		isUseHandle_[i] = false;
	}

#ifdef _DEBUG
	isUseHandle_[0] = true;
#endif // _DEBUG


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
		return currentHandleIndex;
	}
	assert(currentHandleIndex < heapSize);
	if (currentHandleIndex >= heapSize /*|| isUse[currentHadleIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	isUseHandle_[currentHandleIndex] = true;
	tex->CreateSRVView(heapHandles[currentHandleIndex].first);

	//isUse[currentHadleIndex] = true;

	currentHandleIndex++;

	return currentHandleIndex - 1u;
}
void ShaderResourceHeap::CreateTxtureView(Texture* tex, uint32_t heapIndex) {
	assert(tex != nullptr);
	assert(heapIndex < heapSize);
	if (currentHandleIndex >= heapSize/* || isUse[heapIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return;
	}
	isUseHandle_[heapIndex] = true;
	tex->CreateSRVView(heapHandles[heapIndex].first);

	//isUse[heapIndex] = true;
}

uint32_t ShaderResourceHeap::CreatePerarenderView(RenderTarget& renderTarget) {
	assert(currentHandleIndex < heapSize);
	if (currentHandleIndex >= heapSize/* || isUse[currentHadleIndex]*/) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatePerarenderView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	isUseHandle_[currentHandleIndex] = true;
	renderTarget.CreateView(heapHandles[currentHandleIndex].first, heapHandles[currentHandleIndex].second, currentHandleIndex);
	currentHandleIndex++;

	//isUse[currentHadleIndex] = true;

	return currentHandleIndex - 1u;
}

void ShaderResourceHeap::BookingHeapPos(UINT nextCreateViewNum) {
	bool isLoop = false;
	auto heapHandleItr = std::find(isUseHandle_.begin(), isUseHandle_.end(), false);
	do {
		if (heapHandleItr == isUseHandle_.end()) {
			ErrorCheck::GetInstance()->ErrorTextBox("SetReleasedIndexPos failed : all view used", "ShaderResourceHeap");
			return;
		}


		size_t startIndex = std::distance(isUseHandle_.begin(), heapHandleItr);
		size_t index = startIndex;

		for (UINT i = 0u; i < nextCreateViewNum * 10; i++) {
			if (isUseHandle_[index]) {
				isLoop = true;
				for (UINT j = i; j < nextCreateViewNum * 10; j++) {
					index++;
				}
				startIndex = index;
				break;
			}
			else {
				index++;
			}
		}

		if (startIndex != index) {
			isLoop = false;
		}

		if (!isLoop) {
			currentHandleIndex = static_cast<UINT>(startIndex);
			return;
		}

		heapHandleItr = isUseHandle_.begin();
		std::advance(heapHandleItr, index);
	} while (isLoop);
}


void ShaderResourceHeap::ReleaseView(UINT viewHandle) {
	isUseHandle_[viewHandle] = false;
}