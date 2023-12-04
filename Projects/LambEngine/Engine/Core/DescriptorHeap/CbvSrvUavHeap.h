#pragma once
#include "DescriptorHeap.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Buffer/StructuredBuffer/StructuredBuffer.h"
#include "TextureManager/Texture/Texture.h"
#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

/// <summary>
/// ディスクリプタヒープ管理クラス
/// </summary>
class CbvSrvUavHeap final : public DescriptorHeap {
private:
	CbvSrvUavHeap() = delete;
	CbvSrvUavHeap(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap(CbvSrvUavHeap&& right) noexcept = delete;
	CbvSrvUavHeap(UINT numDescriptor);
	~CbvSrvUavHeap();
	CbvSrvUavHeap& operator=(const CbvSrvUavHeap& right) = delete;
	CbvSrvUavHeap& operator=(CbvSrvUavHeap&& right) noexcept = delete;

public:
	static void Initialize(UINT heapSize);

	static void Finalize();

	static CbvSrvUavHeap* GetInstance();

private:
	static CbvSrvUavHeap* instance_;

private:
	void CreateDescriptorHeap(uint32_t heapSize) override;

public:
	void SetHeap();
	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex);
	void Use(uint32_t handleIndex, UINT rootParmIndex);

public:
	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="conBuf">ConstBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<Lamb::IsNotPtr T>
	uint32_t CreateConstBufferView(ConstBuffer<T>& conBuf) {
		assert(currentHandleIndex_ < heapSize_);
		if (currentHandleIndex_ >= heapSize_) {
			Lamb::ErrorLog("Over HeapSize", "CreateConstBufferView()","CbvSrvUavHeap");
		}

		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex_);
			conBuf.CrerateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
			currentHandleIndex_++;
			return currentHandleIndex_ - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			conBuf.CrerateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
			bookingHandle_.pop_front();
			return nowCreateViewHandle;
		}

	}

	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="conBuf">ConstBuffer</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	template<Lamb::IsNotPtr T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf, UINT heapIndex) {
		assert(heapIndex < heapSize_);
		if (heapIndex >= heapSize_) {
			Lamb::ErrorLog("Over HeapSize", "CreateConstBufferView()", "CbvSrvUavHeap");
		}

		conBuf.CrerateView(heapHandles_[heapIndex].first, heapHandles_[heapIndex].second, heapIndex);
	}

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="strcBuf">StructuredBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<Lamb::IsNotPtr T>
	uint32_t CreateStructuredBufferView(StructuredBuffer<T>& strcBuf) {
		assert(currentHandleIndex_ < heapSize_);
		if (currentHandleIndex_ >= heapSize_) {
			Lamb::ErrorLog("Over HeapSize", "CreateStructuredBufferView()", "CbvSrvUavHeap");
		}
		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex_);
			strcBuf.CrerateView(heapHandles_[currentHandleIndex_].first, heapHandles_[currentHandleIndex_].second, currentHandleIndex_);
			currentHandleIndex_++;
			return currentHandleIndex_ - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			strcBuf.CrerateView(heapHandles_[nowCreateViewHandle].first, heapHandles_[nowCreateViewHandle].second, nowCreateViewHandle);
			bookingHandle_.pop_front();
			return nowCreateViewHandle;
		}
	}

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="strcBuf">StructuredBuffe</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	template<Lamb::IsNotPtr T>
	void CreateStructuredBufferView(StructuredBuffer<T>& strcBuf, UINT heapIndex) {
		assert(heapIndex < heapSize_);
		if (heapIndex >= heapSize_) {
			Lamb::ErrorLog("Over HeapSize", "CreateStructuredBufferView()", "CbvSrvUavHeap");
		}
		
		strcBuf.CrerateView(heapHandles_[heapIndex].first, heapHandles_[heapIndex].second, heapIndex);
	}

	/// <summary>
	/// テクスチャのビューを作成
	/// </summary>
	/// <param name="tex">Texture</param>
	/// <returns>作成した場所のハンドル</returns>
	uint32_t CreateTxtureView(Texture* tex);

	/// <summary>
	/// テクスチャのビューを作成
	/// </summary>
	/// <param name="tex">Texture</param>
	/// <param name="heapIndex">作成する場所のハンドル</param>
	void CreateTxtureView(Texture* tex, uint32_t heapIndex);

	/// <summary>
	/// ペラポリ用のビュー作成
	/// </summary>
	/// <param name="renderTarget">RenderTarget</param>
	/// <returns>作成した場所のハンドル</returns>
	uint32_t CreatePerarenderView(RenderTarget& renderTarget);

	uint32_t CreateDepthTextureView(class DepthBuffer& depthBuffer);
};