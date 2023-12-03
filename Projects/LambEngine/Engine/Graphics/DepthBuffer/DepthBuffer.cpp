#include "DepthBuffer.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "TextureManager/Texture/Texture.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <cmath>

DepthBuffer::DepthBuffer():
	tex_{},
	depthStencilResource_{},
	srvDesc_{},
	handle_{},
	hadleUINT_{},
	srvHandleCPU_{},
	srvHandleGPU_{},
	srvHadleUINT_{}
{
	DirectXDevice* const directXDevice = DirectXDevice::GetInstance();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource_ = directXDevice->CreateDepthStencilTextureResource(WindowFactory::GetInstance()->GetClientSize());
	assert(depthStencilResource_);
	if (!depthStencilResource_) {
		assert(!"depthStencilResource failed");
		Log::ErrorLog("something error","CreateDepthStencilTextureResource()", "DepthBuffer");
		return;
	}

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

DepthBuffer::DepthBuffer(const Vector2& bufSize):
	tex_{},
	depthStencilResource_{},
	srvDesc_{},
	handle_{},
	hadleUINT_{},
	srvHandleCPU_{},
	srvHandleGPU_{},
	srvHadleUINT_{}
{
	DirectXDevice* const directXDevice = DirectXDevice::GetInstance();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource_ = directXDevice->CreateDepthStencilTextureResource(bufSize);
	assert(depthStencilResource_);
	if (!depthStencilResource_) {
		assert(!"depthStencilResource failed");
		Log::ErrorLog("something error", "CreateDepthStencilTextureResource()", "DepthBuffer");
		return;
	}

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

DepthBuffer::~DepthBuffer() {
	if (depthStencilResource_) {
		depthStencilResource_->Release();
		depthStencilResource_.Reset();
	}
}

void DepthBuffer::CreateDepthView(D3D12_CPU_DESCRIPTOR_HANDLE handle, uint32_t hadleUINT) {
	handle_ = handle;
	hadleUINT_ = hadleUINT;

	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, handle_);
}
void DepthBuffer::CreateSRView(D3D12_CPU_DESCRIPTOR_HANDLE heapHandle, D3D12_GPU_DESCRIPTOR_HANDLE heapHandleGPU, UINT heapHandleUINT) {
	ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();

	device->CreateShaderResourceView(
		depthStencilResource_.Get(),
		&srvDesc_,
		heapHandle
	);

	srvHandleCPU_ = heapHandle;
	srvHandleGPU_ = heapHandleGPU;
	srvHadleUINT_ = heapHandleUINT;

	tex_.reset();
	tex_ = std::make_unique<Texture>();
	assert(tex_);
	tex_->Set(
		depthStencilResource_.Get(),
		srvDesc_,
		srvHandleGPU_,
		srvHadleUINT_
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthBuffer::GetDepthHandle() const {
	return handle_;
}
Texture* const DepthBuffer::GetTex() const {
	return tex_.get();
}