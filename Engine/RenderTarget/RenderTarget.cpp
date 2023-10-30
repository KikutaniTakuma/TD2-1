#include "RenderTarget.h"
#include "Engine/Engine.h"
#include "Engine/EngineParts/Direct3D/Direct3D.h"
#include "Engine/EngineParts/Direct12/Direct12.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <cassert>
#include "Utils/Math/Vector4.h"

RenderTarget::RenderTarget():
	resource(),
	RTVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(Engine::GetInstance()->clientWidth),
	height(Engine::GetInstance()->clientHeight),
	srvDesc{}
{
	auto resDesc = Direct12::GetInstance()->GetSwapchainBufferDesc();

	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	float clsValue[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (int32_t i = 0; i < _countof(clearValue.Color); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties, 
			D3D12_HEAP_FLAG_NONE, 
			&resDesc, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			&clearValue, 
			IID_PPV_ARGS(resource.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateCommittedResource Function Failed", "RenderTarget");
		return;
	}

	RTVHeap = Direct3D::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	device->
		CreateRenderTargetView(
			resource.Get(),
			&rtvDesc,
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::RenderTarget(uint32_t width_, uint32_t height_) :
	resource(),
	RTVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(width_),
	height(height_),
	srvDesc{}
{
	auto resDesc = Direct12::GetInstance()->GetSwapchainBufferDesc();
	resDesc.Width = width;
	resDesc.Height = height;


	// Resourceを生成する
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	float clsValue[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (int32_t i = 0; i < _countof(clearValue.Color); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();

	// 実際にリソースを作る
	HRESULT hr = device->
		CreateCommittedResource(
			&heapPropaerties,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(resource.GetAddressOf())
		);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateCommittedResource Function Failed", "RenderTarget");
		return;
	}

	RTVHeap = Direct3D::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	device->
		CreateRenderTargetView(
			resource.Get(),
			&rtvDesc,
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
}

RenderTarget::~RenderTarget() {
	if (RTVHeap) {
		RTVHeap->Release();
	}
}

void RenderTarget::SetThisRenderTarget() {
	isResourceStateChange = false;

	Direct12::GetInstance()->Barrier(
		resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	auto rtvHeapHandle = RTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = Engine::GetDsvHandle();
	Direct12::GetInstance()->GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);

	Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	Direct12::GetInstance()->GetCommandList()->ClearRenderTargetView(rtvHeapHandle, clearColor.m.data(), 0, nullptr);
	//Engine::SetViewPort(width, height);
}

void RenderTarget::ChangeResourceState() {
	if (!isResourceStateChange) {
		Direct12::GetInstance()->Barrier(
			resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		isResourceStateChange = true;
	}
}

void RenderTarget::SetMainRenderTarget() {
	ChangeResourceState();
	
	Direct12::GetInstance()->SetMainRenderTarget();
}

void RenderTarget::UseThisRenderTargetShaderResource() {
	static auto mainComList = Direct12::GetInstance()->GetCommandList();
	mainComList->SetGraphicsRootDescriptorTable(0, srvHeapHandle);
}

void RenderTarget::CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, UINT descHeapHandleUINT) {
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	
	device->CreateShaderResourceView(
		resource.Get(),
		&srvDesc,
		descHeapHandle
	);

	srvHeapHandle = descHeapHandleGPU;
	srvHeapHandleUint = descHeapHandleUINT;
}