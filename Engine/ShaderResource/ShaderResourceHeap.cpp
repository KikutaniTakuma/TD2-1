#include "ShaderResourceHeap.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/EngineParts/Direct3D/Direct3D.h"
#include "Engine/EngineParts/Direct12/Direct12.h"
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
	heapHandles(0),
	releaseHandle_(),
	useHandle_(),
	bookingHandle_()
{
	SRVHeap = Direct3D::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	UINT incrementSRVCBVUAVHeap = Direct3D::GetInstance()->GetIncrementSRVCBVUAVHeap();

	heapHandles.reserve(heapSize);
	heapHandles.push_back({ SRVHeap->GetCPUDescriptorHandleForHeapStart(),
							SRVHeap->GetGPUDescriptorHandleForHeapStart() });
	auto heapHandleFirstItr = heapHandles.begin();
	for (uint32_t i = 1; i < heapSize; i++) {
		auto hadleTmp = *heapHandleFirstItr;
		hadleTmp.first.ptr += incrementSRVCBVUAVHeap * i;
		hadleTmp.second.ptr += incrementSRVCBVUAVHeap * i;
		heapHandles.push_back(hadleTmp);
	}
	bookingHandle_.clear();

#ifdef _DEBUG
	useHandle_.push_back(0u);
#endif // _DEBUG
}

ShaderResourceHeap::~ShaderResourceHeap() {
	Reset();
}

void ShaderResourceHeap::SetHeap() {
	static auto commandlist = Direct12::GetInstance()->GetCommandList();
	commandlist->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
}

void ShaderResourceHeap::Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex) {
	static auto commandlist = Direct12::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, handle);
}

void ShaderResourceHeap::Use(uint32_t handleIndex, UINT rootParmIndex) {
	auto commandlist = Direct12::GetInstance()->GetCommandList();
	commandlist->SetGraphicsRootDescriptorTable(rootParmIndex, heapHandles[handleIndex].second);
}

void ShaderResourceHeap::Reset() {
	if (SRVHeap) {
		SRVHeap->Release();
		SRVHeap.Reset();
	}
}

uint32_t ShaderResourceHeap::CreateTxtureView(Texture* tex) {
	assert(tex != nullptr);
	if (tex == nullptr || !*tex) {
		return currentHandleIndex;
	}
	assert(currentHandleIndex < heapSize);
	if (currentHandleIndex >= heapSize) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex);
		tex->CreateSRVView(heapHandles[currentHandleIndex].first);
		currentHandleIndex++;
		return currentHandleIndex - 1u;
	}
	// もしリリースした場所に作るなら
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		tex->CreateSRVView(heapHandles[nowCreateViewHandle].first);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

void ShaderResourceHeap::CreateTxtureView(Texture* tex, uint32_t heapIndex) {
	assert(tex != nullptr);
	assert(heapIndex < heapSize);
	if (currentHandleIndex >= heapSize) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		return;
	}

	tex->CreateSRVView(heapHandles[heapIndex].first);
}

uint32_t ShaderResourceHeap::CreatePerarenderView(RenderTarget& renderTarget) {
	assert(currentHandleIndex < heapSize);
	if (currentHandleIndex >= heapSize) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreatePerarenderView failed\nOver HeapSize", "ShaderResourceHeap");
		return std::numeric_limits<uint32_t>::max();
	}

	if (bookingHandle_.empty()) {
		useHandle_.push_back(currentHandleIndex);
		renderTarget.CreateView(heapHandles[currentHandleIndex].first, heapHandles[currentHandleIndex].second, currentHandleIndex);
		currentHandleIndex++;
		return currentHandleIndex - 1u;
	}
	else {
		uint32_t nowCreateViewHandle = bookingHandle_.front();
		useHandle_.push_back(nowCreateViewHandle);
		renderTarget.CreateView(heapHandles[nowCreateViewHandle].first, heapHandles[nowCreateViewHandle].second, nowCreateViewHandle);
		bookingHandle_.pop_front();
		return nowCreateViewHandle;
	}
}

void ShaderResourceHeap::BookingHeapPos(UINT nextCreateViewNum) {
	// リリースハンドルがないなら予約しない
	if (releaseHandle_.empty()) {
		bookingHandle_.clear();
		return;
	}
	// ディスクリプタが一つも作られてない場合は0を指定
	if (useHandle_.empty()) {
		currentHandleIndex = 0u;
		return;
	}

	// リリースハンドルがの最初のイテレータ
	auto releaseHandle = releaseHandle_.begin();
	// 予約済みを使えるかどうか
	bool isCanUseHandle = true;

	do {
		// ループするのでここでも初期化
		isCanUseHandle = true;
		// 予約済みのハンドルをクリア
		bookingHandle_.clear();

		// 予約する数分格納
		if (nextCreateViewNum != 0u) {
			bookingHandle_.push_back(*releaseHandle);
		}
		for (uint32_t i = 1; i < nextCreateViewNum; i++) {
			releaseHandle++;
			// 格納中にリリースハンドルが最後まで行ってしまったら
			// 解放しループを抜ける
			if (releaseHandle == releaseHandle_.end()) {
				bookingHandle_.clear();
				break;
			}
			bookingHandle_.push_back(*releaseHandle);
		}

		// 予約出来なかったらループを抜けて早期リターン
		if (bookingHandle_.empty()) {
			return;
		}

		// 予約したものが連続したハンドルか
		for (size_t i = 0; i < bookingHandle_.size() - 1; i++) {
			// 連続してない場合予約ハンドルを解放してループを抜ける
			if (bookingHandle_[i] != bookingHandle_[i + 1llu]) {
				isCanUseHandle = false;
				bookingHandle_.clear();
				break;
			}
		}

		// ちゃんと予約出来るまでループ
	} while (!isCanUseHandle);

	for (auto& i : bookingHandle_) {
		// 予約済みになったリリースハンドルを解放
		std::erase(releaseHandle_, i);
		// 予約済みになったUseハンドルを解放
		std::erase(useHandle_, i);
	}
}


void ShaderResourceHeap::ReleaseView(UINT viewHandle) {
	if (!releaseHandle_.empty()) {
		// リリースハンドルにすでに格納されているか
		auto isReleased = std::find(releaseHandle_.begin(), releaseHandle_.end(), viewHandle);
		if (isReleased != releaseHandle_.end()) {
			// すでにリリースハンドルがある
			return;
		}
	}
	// Useハンドルの中に今から解放するハンドルがあるか
	auto isReleased = std::find(useHandle_.begin(), useHandle_.end(), viewHandle);
	if (isReleased == useHandle_.end()) {
		// そもそも解放するディスクリプタがないので早期リターン
		return;
	}

	// リリースハンドルに格納
	releaseHandle_.push_back(viewHandle);
}