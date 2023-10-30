#include "Direct12.h"
#include "Engine/Engine.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/EngineParts/Direct3D/Direct3D.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include <cassert>

#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);
#endif // _DEBUG

Direct12* Direct12::instance_ = nullptr;

Direct12* Direct12::GetInstance() {
	assert(instance_);
	return instance_;
}

void Direct12::Initialize() {
	instance_ = new Direct12{};
}
void Direct12::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

Direct12::Direct12():
	commandQueue_{},
	commandAllocator_{},
	commandList_{},
	isCommandListClose_{false},
	swapChain_{},
	swapChainResource_{},
	rtvDescriptorHeap_{},
	rtvHandles_{},
	fence_{},
	fenceVal_{0llu},
	fenceEvent_{nullptr},
	isRenderState_{false}
{
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	static IDXGIFactory7* dxgiFactory = Direct3D::GetInstance()->GetDxgiFactory();
	static UINT incrementRTVHeap = Direct3D::GetInstance()->GetIncrementRTVHeap();

	// コマンドキューを作成
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandQueue() Failed", "Engine");
		return;
	}

	// コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandAllocator() Failed", "Engine");
		return;
	}

	// コマンドリストを作成する
	commandList_ = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateCommandList() Failed", "Engine");
		return;
	}


	// スワップチェーンの作成
	swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue_.Get(), WinApp::GetInstance()->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateSwapChainForHwnd() Failed", "Engine");
		return;
	}

	dxgiFactory->MakeWindowAssociation(
		WinApp::GetInstance()->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);


	// デスクリプタヒープの作成
	rtvDescriptorHeap_ = Direct3D::GetInstance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SwepChainのメモリとディスクリプタと関連付け
	// バックバッファの数を取得
	DXGI_SWAP_CHAIN_DESC backBufferNum{};
	hr = swapChain_->GetDesc(&backBufferNum);
	// SwapChainResource初期化
	swapChainResource_.reserve(backBufferNum.BufferCount);
	swapChainResource_.resize(backBufferNum.BufferCount);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// ディスクリプタの先頭を取得
	rtvHandles_.reserve(backBufferNum.BufferCount);
	rtvHandles_.resize(backBufferNum.BufferCount);
	auto rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < backBufferNum.BufferCount; ++i) {
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(swapChainResource_[i].GetAddressOf()));
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : GetBuffer() Failed", "Engine");
			return;
		}
		rtvHandles_[i].ptr = rtvStartHandle.ptr + (i * incrementRTVHeap);
		device->CreateRenderTargetView(swapChainResource_[i].Get(), &rtvDesc, rtvHandles_[i]);
	}

#ifdef _DEBUG
	// SRV用のヒープ
	auto srvDescriptorHeap = ShaderResourceHeap::GetInstance();

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHwnd());
	ImGui_ImplDX12_Init(
		device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap->Get(),
		srvDescriptorHeap->GetSrvCpuHeapHandle(0),
		srvDescriptorHeap->GetSrvGpuHeapHandle(0)
	);
#endif // DEBUG


	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0llu;
	hr = device->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateFence() Failed", "Engine");
		return;
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
	if (!(fenceEvent_ != nullptr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDirect12() : CreateEvent() Failed", "Engine");
		return;
	}
}

Direct12::~Direct12() {
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG
	CloseHandle(fenceEvent_);
	rtvDescriptorHeap_->Release();
}

void Direct12::WaitForFinishCommnadlist() {
	// Fenceの値を更新
	fenceVal_++;
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);

	// Fenceの値が指定したSigna値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void Direct12::SetViewPort(uint32_t width, uint32_t height) {
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	commandList_->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようになる
	scissorRect.left = 0;
	scissorRect.right = static_cast<LONG>(WinApp::GetInstance()->GetWindowSize().x);
	scissorRect.top = 0;
	scissorRect.bottom = static_cast<LONG>(WinApp::GetInstance()->GetWindowSize().y);
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void Direct12::Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource) {
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = resource;
	// subResourceの設定
	barrier.Transition.Subresource = subResource;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = before;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = after;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
}

void Direct12::SetMainRenderTarget() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	// 描画先をRTVを設定する
	auto dsvH = Engine::GetDsvHandle();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvH);
}

void Direct12::ClearBackBuffer() {
	auto dsvH = Engine::GetDsvHandle();
	commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 指定した色で画面全体をクリアする
	Vector4 clearColor = { 0.1f, 0.25f, 0.5f, 0.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor.m.data(), 0, nullptr);
}

void Direct12::ChangeBackBufferState() {
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	if (isRenderState_) {
		Barrier(
			swapChainResource_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		isRenderState_ = false;
	}
	else {
		Barrier(
			swapChainResource_[backBufferIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		isRenderState_ = true;
	}
}

void Direct12::CloseCommandlist() {
	// コマンドリストを確定させる
	HRESULT hr = commandList_->Close();
	isCommandListClose_ = true;
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Close() Failed", "Engine");
	}
}

void Direct12::ExecuteCommandLists() {
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void Direct12::SwapChainPresent() {
	// GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);
}

void Direct12::ResetCommandlist() {
	// 次フレーム用のコマンドリストを準備
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
	}
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	isCommandListClose_ = false;
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
	}
}