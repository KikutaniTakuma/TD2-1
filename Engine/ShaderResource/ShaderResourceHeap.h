#pragma once
#include <memory>
#include <vector>
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

	template<IsNotPtrCB T>
	uint32_t CreateConstBufferView(ConstBuffer<T>& conBuf) {
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHadles[currentHadleIndex].first, heapHadles[currentHadleIndex].second, currentHadleIndex);
		currentHadleIndex++;

		return currentHadleIndex - 1u;
	}

	template<IsNotPtrCB T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf, UINT heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHadles[heapIndex].first, heapHadles[heapIndex].second, heapIndex);
	}

	template<IsNotPtrSB T>
	uint32_t CreateStructuredBufferView(StructuredBuffer<T>& strcBuf) {
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		strcBuf.CrerateView(heapHadles[currentHadleIndex].first, heapHadles[currentHadleIndex].second, currentHadleIndex);
		currentHadleIndex++;

		return currentHadleIndex - 1u;
	}

	template<IsNotPtrSB T>
	void CreateStructuredBufferView(StructuredBuffer<T>& strcBuf, UINT heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		strcBuf.CrerateView(heapHadles[heapIndex].first, heapHadles[heapIndex].second, heapIndex);
	}

	inline uint32_t CreateTxtureView(Texture* tex) {
		assert(tex != nullptr);
		if (tex == nullptr || !*tex) {
			return currentHadleIndex;
		}
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		tex->CreateSRVView(heapHadles[currentHadleIndex].first);
		currentHadleIndex++;

		return currentHadleIndex - 1u;
	}
	inline void CreateTxtureView(Texture* tex, uint32_t heapIndex) {
		assert(tex != nullptr);
		assert(heapIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		tex->CreateSRVView(heapHadles[heapIndex].first);
	}

	inline uint32_t CreatePerarenderView(RenderTarget& renderTarget) {
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreatePerarenderView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		renderTarget.CreateView(heapHadles[currentHadleIndex].first, heapHadles[currentHadleIndex].second);
		currentHadleIndex++;

		return currentHadleIndex - 1u;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHeapHandle(uint32_t heapIndex) {
		return heapHadles[heapIndex].first;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHeapHandle(uint32_t heapIndex) {
		return heapHadles[heapIndex].second;
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

private:
	void Reset();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	UINT heapSize;
	UINT currentHadleIndex;

	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHadles;
};