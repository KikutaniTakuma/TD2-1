#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/StructuredBuffer/StructuredBuffer.h"
#include "TextureManager/Texture/Texture.h"
#include <memory>
#include <vector>
#include "Engine/ErrorCheck/ErrorCheck.h"

class ShaderResourceHeap {
private:
	enum class HeapType {
		SRV,
		CBV,
		UAV
	};

public:
	ShaderResourceHeap();
	ShaderResourceHeap(const ShaderResourceHeap& right);
	ShaderResourceHeap(ShaderResourceHeap&& right) noexcept;
	ShaderResourceHeap(uint16_t numDescriptor);
	~ShaderResourceHeap();
public:
	ShaderResourceHeap& operator=(const ShaderResourceHeap& right);
	ShaderResourceHeap& operator=(ShaderResourceHeap&& right) noexcept;

public:
	void InitializeReset();
	void InitializeReset(uint32_t numDescriptor);

	void Use();
	void Use(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	template<class T>
	uint32_t CreateConstBufferView(ConstBuffer<T>& conBuf) {
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHadles[currentHadleIndex].first);
		currentHadleIndex++;

		heapOrder.push_back(HeapType::CBV);

		return currentHadleIndex - 1u;
	}

	template<class T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf, uint32_t heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateConstBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		conBuf.CrerateView(heapHadles[heapIndex].first);

		heapOrder[heapIndex] = HeapType::CBV;
	}

	template<class T>
	uint32_t CreateStructuredBufferView(StructuredBuffer<T>& strcBuf) {
		assert(currentHadleIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		strcBuf.CrerateView(heapHadles[currentHadleIndex].first);
		currentHadleIndex++;

		heapOrder.push_back(HeapType::SRV);

		return currentHadleIndex - 1u;
	}

	template<class T>
	void CreateStructuredBufferView(StructuredBuffer<T>& strcBuf, uint32_t heapIndex) {
		assert(heapIndex < heapSize);
		if (heapIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreateStructuredBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}

		strcBuf.CrerateView(heapHadles[heapIndex].first);

		heapOrder[heapIndex] = HeapType::SRV;
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

		heapOrder.push_back(HeapType::SRV);

		return currentHadleIndex - 1u;
	}
	inline void CreateTxtureView(Texture* tex, uint32_t heapIndex) {
		assert(tex != nullptr);
		assert(heapIndex < heapSize);
		if (currentHadleIndex >= heapSize) {
			ErrorCheck::GetInstance()->ErrorTextBox("CreatTxtureBufferView failed\nOver HeapSize", "ShaderResourceHeap");
		}
		tex->CreateSRVView(heapHadles[heapIndex].first);

		heapOrder[heapIndex] = HeapType::SRV;
	}

	D3D12_ROOT_PARAMETER GetParameter();

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

	void Reset();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	UINT heapSize;
	UINT currentHadleIndex;

	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> heapHadles;
	std::vector<HeapType> heapOrder;

	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
};