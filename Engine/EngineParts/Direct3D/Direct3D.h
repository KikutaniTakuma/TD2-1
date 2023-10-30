#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <cstdint>

class Direct3D {
private:
	Direct3D();
	Direct3D(const Direct3D&) = delete;
	Direct3D(Direct3D&&) = delete;
	~Direct3D() = default;

public:
	Direct3D operator=(const Direct3D&) = delete;
	Direct3D operator=(Direct3D&&) = delete;

public:
	static Direct3D* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static Direct3D* instance_;

public:
	ID3D12DescriptorHeap* CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
	);

	ID3D12Resource* CreateBufferResuorce(size_t sizeInBytes);

	ID3D12Resource* CreateDepthStencilTextureResource(int32_t width, int32_t height);

public:
	inline UINT GetIncrementSRVCBVUAVHeap() const {
		return incrementSRVCBVUAVHeap_;
	}

	inline UINT GetIncrementRTVHeap() const {
		return incrementRTVHeap_;
	}

	inline UINT GetIncrementDSVHeap() const {
		return incrementDSVHeap_;
	}

	inline UINT GetIncrementSAMPLER() const {
		return incrementSAMPLER_;
	}

	ID3D12Device* GetDevice() const {
		return device_.Get();
	}

	IDXGIFactory7* GetDxgiFactory() const {
		return dxgiFactory_.Get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

	UINT incrementSRVCBVUAVHeap_;
	UINT incrementRTVHeap_;
	UINT incrementDSVHeap_;
	UINT incrementSAMPLER_;
};