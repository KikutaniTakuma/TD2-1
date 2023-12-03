#pragma once
#include <memory>
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#undef max
#undef min

class DepthBuffer {
public:
	DepthBuffer();
	DepthBuffer(const class Vector2& bufSize);
	DepthBuffer(const DepthBuffer&) = delete;
	DepthBuffer(DepthBuffer&&) = delete;
	~DepthBuffer();

	DepthBuffer& operator=(const DepthBuffer&) = delete;
	DepthBuffer& operator=(DepthBuffer&&) = delete;

public:
	void CreateDepthView(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t hadleUINT);
	void CreateSRView(D3D12_CPU_DESCRIPTOR_HANDLE heapHandle, D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU, UINT heapHandleUINT);
	
public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthHandle() const;
	class Texture* const GetTex() const;

private:
	std::unique_ptr<class Texture> tex_;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	D3D12_CPU_DESCRIPTOR_HANDLE handle_;
	uint32_t hadleUINT_;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
	uint32_t srvHadleUINT_;
};