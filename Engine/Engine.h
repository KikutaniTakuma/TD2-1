#pragma once
#include <Windows.h>
#undef max
#undef min
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include <wrl.h>
#if DIRECTINPUT_VERSION != 0x0800
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
#pragma comment(lib, "DirectXTK12.lib")

#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <unordered_map>

class Vector2;
class Vector3;
class Vector4;
class Mat4x4;


class Engine {
public:
	enum class Resolution {
		HD,   // 1280x720  720p
		FHD,  // 1920x1080 1080p
		UHD,  // 2560x1440 4K
		SHV,  // 3840x2160 8K

		User, // Userのディスプレイ環境の解像度(メインディスプレイがFHDならFHDになる)。又、Debug時フルスクリーンならこれになる

		ResolutionNum // 設定では使わない(これをセットした場合FHDになる)
	};

public:
	/// <summary>
	/// 静的関数
	/// </summary>
	static ID3D12DescriptorHeap* CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderrVisible
	);

	static ID3D12Resource* CreateBufferResuorce(size_t sizeInBytes);

	static ID3D12Resource* CreateDepthStencilTextureResource(int32_t width, int32_t height);

	static void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource = 0);

private:
	Engine() = default;
	~Engine();

public:
	/// <summary>
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowName">Windowの名前</param>
	/// <param name="resolution">画質設定</param>
	static bool Initialize(const std::string& windowName, Resolution resolution = Resolution::User);

	static void Finalize();

private:
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static Engine* engine;

public:
	static inline Engine* GetInstance() {
		return engine;
	}

	static ID3D12GraphicsCommandList* GetCommandList() {
		return engine->commandList.Get();
	}

	static ID3D12CommandQueue* GetCommandQueue() {
		return engine->commandQueue.Get();
	}

	static inline ID3D12Device* GetDevice() {
		return engine->device.Get();
	}

	static inline ID3D12DescriptorHeap* GetDSVHeap() {
		return engine->dsvHeap.Get();
	}

	static inline D3D12_DESCRIPTOR_HEAP_DESC GetMainRTVDesc() {
		return engine->rtvDescriptorHeap->GetDesc();
	}

	static inline D3D12_RESOURCE_DESC GetSwapchainBufferDesc() {
		return engine->swapChainResource[0]->GetDesc();
	}

	static inline D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() {
		return engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	static inline D3D12_CPU_DESCRIPTOR_HANDLE GetMainRendertTargetHandle() {
		UINT backBufferIndex = engine->swapChain->GetCurrentBackBufferIndex();
		return engine->rtvHandles[backBufferIndex];
	}

	static inline DirectX::SpriteFont* GetFont(const std::string& fontName) {
		return engine->spriteFonts[fontName].get();
	}

	static inline DirectX::SpriteBatch* GetBatch(const std::string& fontName) {
		return engine->spriteBatch[fontName].get();
	}

	static inline Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetFontHeap(const std::string& fontName) {
		return engine->fontHeap[fontName];
	}

	static inline bool GetIsCloseCommandList() {
		return engine->isCommandListClose;
	}


	///
	/// Window生成用
	/// 
public:
	int32_t clientWidth = 0;
	int32_t clientHeight = 0;



#ifdef _DEBUG
	///
	/// Debug用
	/// 
private:
	class Debug {
	public:
		Debug();
		~Debug();

	public:
		void InitializeDebugLayer();

	private:
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	};

	static Debug debugLayer;
#endif




	/// 
	/// Dirct3D
	/// 
private:
	bool InitializeDirect3D();

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter;

	static UINT incrementSRVCBVUAVHeap;
	static UINT incrementRTVHeap;
	static UINT incrementDSVHeap;
	static UINT incrementSAMPLER;
public:
	static UINT GetIncrementSRVCBVUAVHeap() {
		return incrementSRVCBVUAVHeap;
	}

	static UINT GetIncrementRTVHeap() {
		return incrementRTVHeap;
	}

	static UINT GetIncrementDSVHeap() {
		return incrementDSVHeap;
	}

	static UINT GetIncrementSAMPLER() {
		return incrementSAMPLER;
	}




	/// 
	/// DirectX12
	/// 
private:
	bool InitializeDirect12();

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	bool isCommandListClose = false;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResource;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceVal = 0;
	HANDLE fenceEvent = nullptr;
	


/// <summary>
/// 文字表示関係
/// </summary>
private:
	bool InitializeSprite();

public:
	static void LoadFont(const std::string& formatName);

private:
	std::unique_ptr<DirectX::GraphicsMemory> gmemory;
	std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteFont>> spriteFonts;
	std::unordered_map<std::string, std::unique_ptr<DirectX::SpriteBatch>> spriteBatch;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> fontHeap;




	/// 
	/// 描画関係
	/// 
private:
	bool InitializeDraw();
	void ChangeResolution();

public:
	static void SetResolution(Resolution set);
	static void SetViewPort(uint32_t widht, uint32_t height);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	Resolution resolution;
	Resolution setResolution;

	///
	/// MainLoop
	/// 
public:
	static bool WindowMassage();

	static void FrameStart();

	static void FrameEnd();
};