#pragma once
#include <memory>
#include <vector>
#include <list>
#include <deque>
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/StructuredBuffer/StructuredBuffer.h"
#include "TextureManager/Texture/Texture.h"
#include "Engine/RenderTarget/RenderTarget.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

class ShaderResourceHeap {
private:
	ShaderResourceHeap() = delete;
	ShaderResourceHeap(const ShaderResourceHeap& right) = delete;
	ShaderResourceHeap(ShaderResourceHeap&& right) noexcept = delete;
	ShaderResourceHeap(UINT numDescriptor);
	~ShaderResourceHeap();
	ShaderResourceHeap& operator=(const ShaderResourceHeap& right) = delete;
	ShaderResourceHeap& operator=(ShaderResourceHeap&& right) noexcept = delete;

public:
	static void Initialize(UINT numDescriptor);

	static void Finalize();

	static ShaderResourceHeap* GetInstance();

private:
	static ShaderResourceHeap* instance;

public:
	void SetHeap();

	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT rootParmIndex);
	void Use(uint32_t handleIndex, UINT rootParmIndex);

	/// <summary>
	/// CBVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="conBuf">ConstBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<IsNotPtrCB T>
	uint32_t CreateConstBufferView(ConstBuffer<T>& conBuf) {
		assert(currentHandleIndex < heapSize);
		if (currentHandleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex);
			conBuf.CrerateView(heapHandles[currentHandleIndex].first, heapHandles[currentHandleIndex].second, currentHandleIndex);
			currentHandleIndex++;
			return currentHandleIndex - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			conBuf.CrerateView(heapHandles[nowCreateViewHandle].first, heapHandles[nowCreateViewHandle].second, nowCreateViewHandle);
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
	template<IsNotPtrCB T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf, UINT heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHandles[heapIndex].first, heapHandles[heapIndex].second, heapIndex);
	}

	/// <summary>
	/// SRVを作成
	/// </summary>
	/// <typeparam name="T">ポインタ型に制限</typeparam>
	/// <param name="strcBuf">StructuredBuffer</param>
	/// <returns>作成した場所のハンドル</returns>
	template<IsNotPtrSB T>
	uint32_t CreateStructuredBufferView(StructuredBuffer<T>& strcBuf) {
		assert(currentHandleIndex < heapSize);
		if (currentHandleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		if (bookingHandle_.empty()) {
			useHandle_.push_back(currentHandleIndex);
			strcBuf.CrerateView(heapHandles[currentHandleIndex].first, heapHandles[currentHandleIndex].second, currentHandleIndex);
			currentHandleIndex++;
			return currentHandleIndex - 1u;
		}
		else {
			uint32_t nowCreateViewHandle = bookingHandle_.front();
			useHandle_.push_back(nowCreateViewHandle);
			strcBuf.CrerateView(heapHandles[nowCreateViewHandle].first, heapHandles[nowCreateViewHandle].second, nowCreateViewHandle);
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
	template<IsNotPtrSB T>
	void CreateStructuredBufferView(StructuredBuffer<T>& strcBuf, UINT heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		
		strcBuf.CrerateView(heapHandles[heapIndex].first, heapHandles[heapIndex].second, heapIndex);
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

	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHeapHandle(uint32_t heapIndex) {
		return heapHandles[heapIndex].first;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHeapHandle(uint32_t heapIndex) {
		return heapHandles[heapIndex].second;
	}
	
	inline UINT GetSize() const {
		return heapSize;
	}

	inline ID3D12DescriptorHeap* Get() const {
		return SRVHeap.Get();
	}

	inline ID3D12DescriptorHeap* const* GetAddressOf() const {
		return SRVHeap.GetAddressOf();
	}

public:
	void BookingHeapPos(UINT nextCreateViewNum);
	void ReleaseView(UINT viewHandle);

private:
	void Reset();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	UINT heapSize;
	UINT currentHandleIndex;

	std::list<uint32_t> releaseHandle_;
	std::list<uint32_t> useHandle_;
	std::deque<uint32_t> bookingHandle_;


	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHandles;
};