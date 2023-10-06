#include "RenderTarget.h"
#include "Engine/Engine.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <cassert>
#include "Utils/Math/Vector4.h"

RenderTarget::RenderTarget():
	resource(),
	RTVHeap(),
	SRVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(Engine::GetInstance()->clientWidth),
	height(Engine::GetInstance()->clientHeight)
{
	auto resDesc = Engine::GetSwapchainBufferDesc();

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

	// 実際にリソースを作る
	HRESULT hr = Engine::GetDevice()->
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

	RTVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	Engine::GetDevice()->
		CreateRenderTargetView(
			resource.Get(), 
			&rtvDesc, 
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Engine::GetDevice()->
		CreateShaderResourceView(
			resource.Get(), 
			&srvDesc, 
			srvHeapHandle
		);

	srvHeapHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

RenderTarget::RenderTarget(uint32_t width_, uint32_t height_) :
	resource(),
	RTVHeap(),
	SRVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(width_),
	height(height_)
{
	auto resDesc = Engine::GetSwapchainBufferDesc();
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

	// 実際にリソースを作る
	HRESULT hr = Engine::GetDevice()->
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

	RTVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	Engine::GetDevice()->
		CreateRenderTargetView(
			resource.Get(),
			&rtvDesc,
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Engine::GetDevice()->
		CreateShaderResourceView(
			resource.Get(),
			&srvDesc,
			srvHeapHandle
		);

	srvHeapHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

RenderTarget::RenderTarget(uint16_t numDescriptor) :
	resource(),
	RTVHeap(),
	SRVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(Engine::GetInstance()->clientWidth),
	height(Engine::GetInstance()->clientHeight)
{
	auto resDesc = Engine::GetSwapchainBufferDesc();

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

	// 実際にリソースを作る
	HRESULT hr = Engine::GetDevice()->
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

	RTVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	Engine::GetDevice()->
		CreateRenderTargetView(
			resource.Get(),
			&rtvDesc,
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Engine::GetDevice()->
		CreateShaderResourceView(
			resource.Get(),
			&srvDesc,
			srvHeapHandle
		);

	srvHeapHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

RenderTarget::RenderTarget(uint16_t numDescriptor, uint32_t width_, uint32_t height_) :
	resource(),
	RTVHeap(),
	SRVHeap(),
	srvHeapHandle(),
	isResourceStateChange(false),
	width(width_),
	height(height_)
{
	auto resDesc = Engine::GetSwapchainBufferDesc();
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

	// 実際にリソースを作る
	HRESULT hr = Engine::GetDevice()->
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

	RTVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	Engine::GetDevice()->
		CreateRenderTargetView(
			resource.Get(),
			&rtvDesc,
			RTVHeap->GetCPUDescriptorHandleForHeapStart()
		);

	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Engine::GetDevice()->
		CreateShaderResourceView(
			resource.Get(),
			&srvDesc,
			srvHeapHandle
		);

	srvHeapHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

RenderTarget::~RenderTarget() {
	if(SRVHeap){
		SRVHeap->Release();
	}

	if (RTVHeap) {
		RTVHeap->Release();
	}
}

void RenderTarget::SetThisRenderTarget() {
	isResourceStateChange = false;

	Engine::Barrier(
		resource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	auto rtvHeapHandle = RTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = Engine::GetDsvHandle();
	Engine::GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);

	Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	Engine::GetCommandList()->ClearRenderTargetView(rtvHeapHandle, clearColor.m.data(), 0, nullptr);
	//Engine::SetViewPort(width, height);
}

void RenderTarget::ChangeResourceState() {
	if (!isResourceStateChange) {
		Engine::Barrier(
			resource.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		);
		isResourceStateChange = true;
	}
}

void RenderTarget::SetMainRenderTarget() {
	ChangeResourceState();
	// 描画先をメインレンダーターゲットに変更
	auto rtvHeapHandle = Engine::GetMainRendertTargetHandle();
	auto dsvH = Engine::GetDsvHandle();
	Engine::GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);
}

void RenderTarget::UseThisRenderTargetShaderResource() {
	Engine::GetCommandList()->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
	auto SRVHandle = SRVHeap->GetGPUDescriptorHandleForHeapStart();
	Engine::GetCommandList()->SetGraphicsRootDescriptorTable(0,SRVHandle);
}