#include "Engine.h"
#include <cassert>
#include <format>
#include <filesystem>

#include "Core/WindowFactory/WindowFactory.h"
#include "Core/DirectXDevice/DirectXDevice.h"
#include "Core/DirectXCommand/DirectXCommand.h"
#include "Core/DirectXSwapChain/DirectXSwapChain.h"
#include "Core/StringOutPutManager/StringOutPutManager.h"
#include "Core/ImGuiManager/ImGuiManager.h"

#include "Core/DescriptorHeap/RtvHeap.h"
#include "Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Core/DescriptorHeap/DsvHeap.h"

#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "MeshManager/MeshManager.h"
#include "Graphics/PipelineManager/PipelineManager.h"

#include "EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/ConvertString/ConvertString.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Engine/EngineUtils/ErrorCheck/ErrorCheck.h"

#include "Utils/Math/Vector2.h"

#include "Graphics/DepthBuffer/DepthBuffer.h"
#include "Utils/ScreenOut/ScreenOut.h"



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
		Log::ErrorLog("CoInitializeEx failed","Initialize()", "Engine");
		return false;
	}

	engine = new Engine();
	assert(engine);

	const auto&& windowTitle = ConvertString(windowName);

	// Window生成
	WindowFactory::GetInstance()->Create(windowTitle, static_cast<int32_t>(windowSize.x), static_cast<int32_t>(windowSize.y));

#ifdef _DEBUG
	// DebugLayer有効化
	debugLayer.InitializeDebugLayer();
#endif

	// デバイス生成
	engine->InitializeDirectXDevice();

	// ディスクリプタヒープ初期化
	RtvHeap::Initialize(128u);
	DsvHeap::Initialize(128u);
	CbvSrvUavHeap::Initialize(4096u);

	// コマンドリスト生成
	engine->InitializeDirectXCommand();

	// スワップチェーン生成
	engine->InitializeDirectXSwapChain();

	ImGuiManager::Initialize();

	if (!engine->InitializeDraw()) {
		Log::ErrorLog("InitializeDraw() Failed","Initialize()", "Engine");
		return false;
	}

	engine->InitializeDirectXTK();

	// 各種マネージャー初期化
	ShaderManager::Initialize();
	TextureManager::Initialize();
	AudioManager::Inititalize();
	PipelineManager::Initialize();
	MeshManager::Initialize();

	return true;
}

void Engine::Finalize() {
	engine->isFinalize = true;

	// 各種マネージャー解放
	MeshManager::Finalize();
	PipelineManager::Finalize();
	AudioManager::Finalize();
	TextureManager::Finalize();
	ShaderManager::Finalize();

	StringOutPutManager::Finalize();

	engine->depthStencil_.reset();

	CbvSrvUavHeap::Finalize();
	DsvHeap::Finalize();
	RtvHeap::Finalize();

	ImGuiManager::Finalize();

	DirectXSwapChain::Finalize();
	DirectXCommand::Finalize();
	DirectXDevice::Finalize();

	delete engine;
	engine = nullptr;

	// COM 終了
	CoUninitialize();
}

bool Engine::IsFinalize() {
	return engine->isFinalize;
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetDsvHandle() {
	return engine->depthStencil_->GetDepthHandle();
}



///
/// DirectXDevice
/// 

void Engine::InitializeDirectXDevice() {
	DirectXDevice::Initialize();
	directXDevice_ = DirectXDevice::GetInstance();
}


/// 
/// DirectXCommand
/// 

void Engine::InitializeDirectXCommand() {
	DirectXCommand::Initialize();
	directXCommand_ = DirectXCommand::GetInstance();
}


/// 
/// DirectXSwapChain
///

void Engine::InitializeDirectXSwapChain() {
	DirectXSwapChain::Initialize();
	directXSwapChain_ = DirectXSwapChain::GetInstance();
}


///
///  DirectXTK
///

void Engine::InitializeDirectXTK() {
	StringOutPutManager::Initialize();
	stringOutPutManager_ = StringOutPutManager::GetInstance();
}




///
/// 描画用
/// 
bool Engine::InitializeDraw() {
	DsvHeap* dsvHeap = DsvHeap::GetInstance();
	if(!dsvHeap) {
		assert(!"CreateDescriptorHeap failed");
		Log::ErrorLog("CreateDescriptorHeap()  Failed","InitializeDraw()", "Engine");
		return false;
	}

	depthStencil_ = std::make_unique<DepthBuffer>();

	dsvHeap->BookingHeapPos(1u);
	dsvHeap->CreateView(*depthStencil_);

	CbvSrvUavHeap* cbvSrvUavHeap = CbvSrvUavHeap::GetInstance();
	cbvSrvUavHeap->BookingHeapPos(1u);
	cbvSrvUavHeap->CreateDepthTextureView(*depthStencil_);

	return true;
}



/// 
/// MianLoop用
/// 

void Engine::FrameStart() {
	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->Start();

	Lamb::screenout.Clear();
	Lamb::screenout << Lamb::endline;

	ImGuiManager::GetInstance()->Start();

	engine->directXSwapChain_->ChangeBackBufferState();
	engine->directXSwapChain_->SetMainRenderTarget();
	engine->directXSwapChain_->ClearBackBuffer();

	// ビューポート
	Vector2 clientSize = WindowFactory::GetInstance()->GetClientSize();
	engine->directXSwapChain_->SetViewPort(static_cast<int32_t>(clientSize.x), static_cast<int32_t>(clientSize.y));

	// SRV用のヒープ
	static auto const srvDescriptorHeap = CbvSrvUavHeap::GetInstance();

	srvDescriptorHeap->SetHeap();
}

void Engine::FrameEnd() {
	static auto err = ErrorCheck::GetInstance();
	if (err->GetError()) {
		return;
	}

	static FrameInfo* const frameInfo = FrameInfo::GetInstance();
	frameInfo->DrawFps();

	Lamb::screenout.Draw();

	ImGuiManager::GetInstance()->End();
	
	engine->directXSwapChain_->ChangeBackBufferState();

	// コマンドリストを確定させる
	engine->directXCommand_->CloseCommandlist();

	// GPUにコマンドリストの実行を行わせる
	engine->directXCommand_->ExecuteCommandLists();


	// GPUとOSに画面の交換を行うように通知する
	engine->directXSwapChain_->SwapChainPresent();

	engine->stringOutPutManager_->GmemoryCommit();

	engine->directXCommand_->WaitForFinishCommnadlist();

	engine->directXCommand_->ResetCommandlist();
	
	// テクスチャの非同期読み込み
	auto textureManager = TextureManager::GetInstance();
	textureManager->ThreadLoadTexture();
	textureManager->ResetCommandList();

	// このフレームで画像読み込みが発生していたら開放する
	// またUnloadされていたらそれをコンテナから削除する
	textureManager->ReleaseIntermediateResource();

	// メッシュの非同期読み込み
	auto meshManager = MeshManager::GetInstance();
	meshManager->ThreadLoad();
	meshManager->JoinThread();
	meshManager->CheckLoadFinish();
	
	// 音の非同期読み込み
	auto audioManager = AudioManager::GetInstance();
	audioManager->ThreadLoad();
	audioManager->CheckThreadLoadFinish();

	frameInfo->End();
}





/// 
/// 各種解放処理なくなっちゃた
/// 
Engine::~Engine() {
	
}