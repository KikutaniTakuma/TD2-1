#include "CbvSrvUavHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Graphics/DepthBuffer/DepthBuffer.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include <numeric>

CbvSrvUavHeap* CbvSrvUavHeap::instance_ = nullptr;

void CbvSrvUavHeap::Initialize(UINT heapSize) {
	instance_ = new CbvSrvUavHeap{ heapSize };
}

void CbvSrvUavHeap::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

CbvSrvUavHeap* CbvSrvUavHeap::GetInstance() {
	return instance_;
}

CbvSrvUavHeap::CbvSrvUavHeap(UINT numDescriptor) :
	DescriptorHeap{}
{
	CreateDescriptorHeap(numDescriptor);

	CreateHeapHandles();

	bookingHandle_.clear();
}

CbvSrvUavHeap::~CbvSrvUavHeap() {
	Reset();
}

void CbvSrvUavHeap::CreateDescriptorHeap(uint32_t heapSize) {
	// 1～(10^6-1)でクランプ
	heapSize_ = std::clamp(heapSize, 1u, static_cast<UINT>(std::pow(10u, 6u)) - 1u);
	heap_ = DirectXDevice::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, heapSize_, true);
}

void CbvSrvUavHeap::SetHeap() {
	static auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetDescriptorHeaps(1, heap_.GetAddressOf());
}
void CbvSrvUavHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex) {
	static auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, handle);
}
void CbvSrvUavHeap::Use(uint32_t handleIndex, UINT rootParmIndex) {
	auto commandlist = DirectXCommand::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHandles_[handleIndex].second);
}

uint32_t CbvSrvUavHeap::CreateTxtureView(Texture* tex) {
	assert(tex != nullptr);
	if (tex == nullptr || !*tex) {
		return currentHandleIndex_;
	}
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		Lamb::ErrorLog("Over HeapSize", "CreateTxtureView()", "CbvSrvUavHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		tex->CreateSRVView(
			heapHandles_[currentHandleIndex_].first,
			heapHandles_[currentHandleIndex_].second,
			currentHandleIndex_
			);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	// もしリリースした場所に作るなら
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		tex->CreateSRVView(
			heapHandles_[nowCreateViewHandle].first,
			heapHandles_[nowCreateViewHandle].second,
			nowCreateViewHandle
		);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

void CbvSrvUavHeap::CreateTxtureView(Texture* tex, uint32_t heapIndex) {
	assert(tex != nullptr);
	assert(heapIndex < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		Lamb::ErrorLog("Over HeapSize", "CreateTxtureView()", "CbvSrvUavHeap");
		return;
	}

	tex->CreateSRVView(
		heapHandles_[heapIndex].first,
		heapHandles_[heapIndex].second,
		heapIndex
		);
}

uint32_t CbvSrvUavHeap::CreatePerarenderView(RenderTarget& renderTarget) {
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		Lamb::ErrorLog("Over HeapSize", "CreatePerarenderView()", "CbvSrvUavHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		renderTarget.CreateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		renderTarget.CreateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

uint32_t CbvSrvUavHeap::CreateDepthTextureView(class DepthBuffer& depthBuffer) {
	assert(currentHandleIndex_ < heapSize_);
	if (currentHandleIndex_ >= heapSize_) {
		Lamb::ErrorLog("Over HeapSize", "CreateDepthTextureView()", "CbvSrvUavHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex_);
		depthBuffer.CreateSRView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
		currentHandleIndex_++;
		return currentHandleIndex_ - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		depthBuffer.CreateSRView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}