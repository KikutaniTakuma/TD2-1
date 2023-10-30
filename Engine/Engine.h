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
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>
#pragma comment(lib, "DirectXTK12.lib")

#include <string>
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

private:
	Engine() = default;
	~Engine();

public:
	/// <summary>
	/// 失敗した場合内部でassertで止められる
	/// </summary>
	/// <param name="windowName">Windowの名前</param>
	/// <param name="windowSize">ウィンドウの大きさ(バックバッファの大きさも同じになる)</param>
	static bool Initialize(const std::string& windowName, const Vector2& windowSize);

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

	static inline ID3D12DescriptorHeap* GetDSVHeap() {
		return engine->dsvHeap.Get();
	}

	static inline D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() {
		return engine->dsvHeap->GetCPUDescriptorHandleForHeapStart();
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
	void InitializeDirect3D();

private:
	class Direct3D* direct3D_ = nullptr;



	/// 
	/// DirectX12
	/// 
private:
	void InitializeDirect12();

private:
	class Direct12* direct12_ = nullptr;
	


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

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	///
	/// MainLoop
	/// 
public:
	static bool WindowMassage();

	static void FrameStart();

	static void FrameEnd();
};