#include "RenderTarget.h"
#include "Engine/Engine.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <cassert>
#include "Utils/Math/Vector4.h"
#include "Engine/Core/DescriptorHeap/RtvHeap.h"

RenderTarget::RenderTarget():
	resource_(),
	isResourceStateChange_(false),
	width_(static_cast<uint32_t>(WindowFactory::GetInstance()->GetClientSize().x)),
	height_(static_cast<uint32_t>(WindowFactory::GetInstance()->GetClientSize().y)),
	srvDesc_{},
	srvHeapHandle_{},
	srvHeapHandleUint_(),
	rtvHeapHandle_{},
	rtvHeapHandleUint_(0u)
{
	auto resDesc = DirectXSwapChain::GetInstance()->GetSwapchainBufferDesc();

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	Vector4 clsValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < clsValue.m.size(); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties, 
			D3D12_HEAP_FLAG_NONE, 
			&resDesc, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			&clearValue, 
			IID_PPV_ARGS(resource_.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("CreateCommittedResource Function Failed", "Constructor", "RenderTarget");
		return;
	}

	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->CreateView(*this);

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::RenderTarget(uint32_t width, uint32_t height) :
	resource_(),
	isResourceStateChange_(false),
	width_(width),
	height_(height),
	srvDesc_{},
	srvHeapHandle_{},
	srvHeapHandleUint_(),
	rtvHeapHandle_{},
	rtvHeapHandleUint_(0u)
{
	auto resDesc = DirectXSwapChain::GetInstance()->GetSwapchainBufferDesc();
	resDesc.Width = width_;
	resDesc.Height = height_;


	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	Vector4 clsValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (size_t i = 0; i < clsValue.m.size(); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(resource_.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("CreateCommittedResource Function Failed", "Constructor", "RenderTarget");
		return;
	}

	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->BookingHeapPos(1u);
	rtvHeap->CreateView(*this);

	srvDesc_ = {};
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc_.Texture2D.MipLevels = 1;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::~RenderTarget() {
	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->ReleaseView(rtvHeapHandleUint_);
}

void RenderTarget::SetThisRenderTarget() {
	isResourceStateChange_ = false;

	Barrier(
		resource_.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	RtvHeap* const rtvHeap = RtvHeap::GetInstance();
	rtvHeap->SetRtv(rtvHeapHandleUint_);

	Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	rtvHeap->ClearRenderTargetView(rtvHeapHandleUint_, clearColor);
}

void RenderTarget::ChangeResourceState() {
	if (!isResourceStateChange_) {
		Barrier(
			resource_.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		isResourceStateChange_ = true;
	}
}

void RenderTarget::SetMainRenderTarget() {
	ChangeResourceState();
	
	DirectXSwapChain::GetInstance()->SetMainRenderTarget();
}

void RenderTarget::UseThisRenderTargetShaderResource() {
	static auto mainComList = DirectXCommand::GetInstance()->GetCommandList();
	mainComList->SetGraphicsRootDescriptorTable(0, srvHeapHandle_);
}

void RenderTarget::CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, UINT descHeapHandleUINT) {
	static ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	
	device->CreateShaderResourceView(
		resource_.Get(),
		&srvDesc_,
		descHeapHandle
	);

	srvHeapHandle_ = descHeapHandleGPU;
	srvHeapHandleUint_ = descHeapHandleUINT;

	tex_.reset();
	tex_ = std::make_unique<Texture>();
	assert(tex_);
	tex_->Set(
		resource_,
		srvDesc_,
		srvHeapHandle_,
		srvHeapHandleUint_
	);
}

void RenderTarget::CreateRTV(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, UINT descHeapHandleUINT) {
	static ID3D12Device* const device = DirectXDevice::GetInstance()->GetDevice();
	
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, descHeapHandle);

	rtvHeapHandle_ = descHeapHandle;
	rtvHeapHandleUint_ = descHeapHandleUINT;
}