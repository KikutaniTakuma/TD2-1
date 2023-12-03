#pragma once

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "../externals/DirectXTex/DirectXTex.h"
#include "../externals/DirectXTex/d3dx12.h"
#include <wrl.h>

#include <string>

#include "Utils/Math/Vector2.h"

/// <summary>
/// テクスチャの読み込みやリソースの管理を担う
/// </summary>
class Texture {
	friend class TextureManager;

public:
	Texture();
	~Texture();

	Texture(const Texture&) = delete;
	Texture(Texture&& tex) noexcept;

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& tex) noexcept;

public:
	inline explicit operator bool() const noexcept {
		return isLoad;
	}

	inline bool operator!() const noexcept {
		return !isLoad;
	}


/// <summary>
/// 画像読み込み用
/// </summary>
private:
	void Load(const std::string& filePath);
	void Load(const std::string& filePath, ID3D12GraphicsCommandList* commandList);
	void Unload();

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	[[nodiscard]]
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12GraphicsCommandList* commandList);

/// <summary>
/// View作成関数
/// </summary>
public:
	void CreateSRVView(
		D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, 
		UINT descHeapHandleUINT
	);

/// <summary>
/// CPU側のリソースを解放
/// </summary>
private:
	void ReleaseIntermediateResource();

public:
	bool CanUse() const {
		return static_cast<bool>(textureResouce) && !static_cast<bool>(intermediateResource) && isLoad;
	}

public:
	void Use(UINT rootParamator);

	void Set(
		const Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
		D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc,
		D3D12_GPU_DESCRIPTOR_HANDLE handle,
		UINT handleUINT
		);


/// <summary>
/// メンバ変数
/// </summary>
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResouce;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHeapHandle;
	uint32_t srvHeapHandleUint;

	bool isLoad;
	bool threadLoadFlg;



	Vector2 size;
public:
	inline const Vector2& getSize() const {
		return size;
	}

private:
	std::string fileName;
public:
	inline const std::string& GetFileName() const {
		return fileName;
	}
};