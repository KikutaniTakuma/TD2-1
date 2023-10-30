#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>

#include "WinApp/WinApp.h"
#include "EngineParts/Direct3D/Direct3D.h"
#include "EngineParts/Direct12/Direct12.h"

#include "ShaderManager/ShaderManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "PipelineManager/PipelineManager.h"

#include "Input/Input.h"
#include "ErrorCheck/ErrorCheck.h"
#include "FrameInfo/FrameInfo.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/Log/Log.h"

#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Line/Line.h"
#include "Drawers/Particle/Particle.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wPram, LPARAM lPram);


#ifdef _DEBUG
Engine::Debug Engine::debugLayer;

Engine::Debug::Debug() :
	debugController(nullptr)
{}

Engine::Debug::~Debug() {
	debugController.Reset();

	// リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}

///
/// デバッグレイヤー初期化
/// 

void Engine::Debug::InitializeDebugLayer() {
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックするようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}

#endif // _DEBUG

/// 
/// 各種初期化処理
/// 

Engine* Engine::engine = nullptr;

bool Engine::Initialize(const std::string& windowName, const Vector2& windowSize) {
	HRESULT hr =  CoInitializeEx(0, COINIT_MULTITHREADED);
	if (hr != S_OK) {
		ErrorCheck::GetInstance()->ErrorTextBox("CoInitializeEx failed", "Engine");
		return false;
	}

	engine = new Engine();
	assert(engine);
	engine->clientWidth = static_cast<int32_t>(windowSize.x);
	engine->clientHeight = static_cast<int32_t>(windowSize.y);

	const auto&& windowTitle = ConvertString(windowName);

	// Window生成
	WinApp::GetInstance()->Create(windowTitle, engine->clientWidth, engine->clientHeight);

#ifdef _DEBUG
	// DebugLayer有効化
	debugLayer.InitializeDebugLayer();
#endif

	// Direct3D生成
	engine->InitializeDirect3D();

	// ディスクリプタヒープ初期化
	ShaderResourceHeap::Initialize(524288);

	// DirectX12生成
	engine->InitializeDirect12();

	if (!engine->InitializeDraw()) {
		ErrorCheck::GetInstance()->ErrorTextBox("Initialize() : InitializeDraw() Failed", "Engine");
		return false;
	}

	if (!engine->InitializeSprite()) {
		ErrorCheck::GetInstance()->ErrorTextBox("Initialize() : InitializeSprite() Failed", "Engine");
		return false;
	}

	Input::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	AudioManager::Inititalize();
	PipelineManager::Initialize();

	Texture2D::Initialize();
	Model::Initialize();
	Line::Initialize();
	Particle::Initialize();

	return true;
}

void Engine::Finalize() {
	Particle::Finalize();
	Texture2D::Finalize();

	PipelineManager::Finalize();
	AudioManager::Finalize();
	TextureManager::Finalize();
	ShaderManager::Finalize();
	Input::Finalize();

	ShaderResourceHeap::Finalize();

	delete engine;
	engine = nullptr;

	// COM 終了
	CoUninitialize();
}





///
/// Direct3D初期化
/// 

void Engine::InitializeDirect3D() {
	Direct3D::Initialize();
	direct3D_ = Direct3D::GetInstance();
}




/// 
/// DirectX12
/// 

void Engine::InitializeDirect12() {
	Direct12::Initialize();
	direct12_ = Direct12::GetInstance();
}





/// <summary>
/// 文字表示関係
/// </summary>
bool Engine::InitializeSprite() {
	static ID3D12Device* device = engine->direct3D_->GetDevice();
	// GraphicsMemory初期化
	gmemory.reset(new DirectX::GraphicsMemory(device));

	return static_cast<bool>(gmemory);
}

void Engine::LoadFont(const std::string& formatName) {
	static ID3D12Device* device = engine->direct3D_->GetDevice();
	if (!std::filesystem::exists(std::filesystem::path(formatName))) {
		ErrorCheck::GetInstance()->ErrorTextBox("Engine::LoadFont() Failed : This file is not exist -> " + formatName, "Engine");
		return;
	}

	engine->fontHeap.insert(
		std::make_pair(
			formatName,
			engine->direct3D_->CreateDescriptorHeap(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true
			)
		)
	);

	DirectX::ResourceUploadBatch resUploadBach(device);
	resUploadBach.Begin();
	DirectX::RenderTargetState rtState(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_D32_FLOAT
	);

	DirectX::SpriteBatchPipelineStateDescription pd(rtState);

	// SpriteFontオブジェクトの初期化
	engine->spriteBatch.insert(
		std::make_pair(formatName, std::make_unique<DirectX::SpriteBatch>(device, resUploadBach, pd)));
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<float>(engine->clientWidth);
	viewport.Height = static_cast<float>(engine->clientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	engine->spriteBatch[formatName]->SetViewport(viewport);

	engine->spriteFonts.insert(
		std::make_pair(
			formatName,
			std::make_unique<DirectX::SpriteFont>(
				device,
				resUploadBach,
				ConvertString(formatName).c_str(),
				engine->fontHeap[formatName]->GetCPUDescriptorHandleForHeapStart(),
				engine->fontHeap[formatName]->GetGPUDescriptorHandleForHeapStart()
			)
		)
	);

	ID3D12CommandQueue* commandQueue = engine->direct12_->GetCommandQueue();
	auto future = resUploadBach.End(commandQueue);

	engine->direct12_->WaitForFinishCommnadlist();

	future.wait();
}






///
/// 描画用
/// 
bool Engine::InitializeDraw() {
	static ID3D12Device* device = engine->direct3D_->GetDevice();

	// DepthStencilTextureをウィンドウサイズで作成
	depthStencilResource = direct3D_->CreateDepthStencilTextureResource(clientWidth, clientHeight);
	assert(depthStencilResource);
	if (!depthStencilResource) {
		assert(!"depthStencilResource failed");
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDraw() : DepthStencilResource Create Failed", "Engine");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	dsvHeap = nullptr;
	if(!SUCCEEDED(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf())))) {
		assert(!"CreateDescriptorHeap failed");
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeDraw() : CreateDescriptorHeap()  Failed", "Engine");
		return false;
	}


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return true;
}



/// 
/// MianLoop用
/// 

bool Engine::WindowMassage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	static auto err = ErrorCheck::GetInstance();

	return (msg.message != WM_QUIT) && !(err->GetError());
}

void Engine::FrameStart() {
	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->Start();

#ifdef _DEBUG
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG

	engine->direct12_->ChangeBackBufferState();
	engine->direct12_->SetMainRenderTarget();
	engine->direct12_->ClearBackBuffer();

	// ビューポート
	engine->direct12_->SetViewPort(engine->clientWidth, engine->clientHeight);

	// SRV用のヒープ
	static auto srvDescriptorHeap = ShaderResourceHeap::GetInstance();

	srvDescriptorHeap->SetHeap();
}

void Engine::FrameEnd() {
	static auto err = ErrorCheck::GetInstance();
	if (err->GetError()) {
		return;
	}


#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList = engine->direct12_->GetCommandList();
	// ImGui描画
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif // DEBUG

	
	engine->direct12_->ChangeBackBufferState();

	// コマンドリストを確定させる
	engine->direct12_->CloseCommandlist();

	// GPUにコマンドリストの実行を行わせる
	engine->direct12_->ExecuteCommandLists();


	// GPUとOSに画面の交換を行うように通知する
	engine->direct12_->SwapChainPresent();
	ID3D12CommandQueue* commandQueue = engine->direct12_->GetCommandQueue();
	engine->gmemory->Commit(commandQueue);

	engine->direct12_->WaitForFinishCommnadlist();

	engine->direct12_->ResetCommandlist();
	
	TextureManager::GetInstance()->ThreadLoadTexture();
	TextureManager::GetInstance()->ResetCommandList();
	

	// このフレームで画像読み込みが発生していたら開放する
	// またUnloadされていたらそれをコンテナから削除する
	TextureManager::GetInstance()->ReleaseIntermediateResource();

	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->End();
}





/// 
/// 各種解放処理
/// 
Engine::~Engine() {
	for (auto& i : fontHeap) {
		i.second->Release();
	}
	depthStencilResource->Release();

	Direct12::Finalize();
	Direct3D::Finalize();
}