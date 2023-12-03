#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include <wrl.h>

#include <string>
#include <memory>
#include <unordered_map>

class Vector2;
class Vector3;
class Vector4;
class Mat4x4;

/// <summary>
/// 各種マネージャーやDirectX12関連、ウィンドウ関連の初期化と解放を担う
/// </summary>
class Engine {
private:
	Engine() = default;
	Engine(const Engine&) = delete;
	Engine(Engine&&) = default;
	~Engine();

	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;

public:
	/// <summary>
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowName">Windowの名前</param>
	/// <param name="windowSize">ウィンドウの大きさ(バックバッファの大きさも同じになる)</param>
	static bool Initialize(const std::string& windowName, const Vector2& windowSize);

	static void Finalize();

	static bool IsFinalize();

private:
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static Engine* engine;

private:
	bool isFinalize;

public:
	static inline Engine* GetInstance() {
		return engine;
	}

	static D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle();

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
	/// DirctXDevice
	/// 
private:
	void InitializeDirectXDevice();

private:
	class DirectXDevice* directXDevice_ = nullptr;

	/// 
	/// DirectXCommand
	/// 
private:
	void InitializeDirectXCommand();

private:
	class DirectXCommand* directXCommand_ = nullptr;

/// 
/// DirectXCommand
/// 
private:
	void InitializeDirectXSwapChain();

private:
	class DirectXSwapChain* directXSwapChain_ = nullptr;

	
/// <summary>
/// DirectXTK
/// </summary>
private:
	void InitializeDirectXTK();

private:
	class StringOutPutManager* stringOutPutManager_ = nullptr;


	/// 
	/// 描画関係
	/// 
private:
	bool InitializeDraw();

private:
	std::unique_ptr<class DepthBuffer> depthStencil_;

	///
	/// MainLoop
	/// 
public:
	static void FrameStart();

	static void FrameEnd();
};