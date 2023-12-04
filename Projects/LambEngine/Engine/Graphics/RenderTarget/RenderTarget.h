#pragma once
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "TextureManager/Texture/Texture.h"

/// <summary>
/// ポストエフェクト用のレンダーターゲットリソース等を管理するクラス
/// </summary>
class RenderTarget {
public:
	RenderTarget();
	RenderTarget(uint32_t width, uint32_t height);
	~RenderTarget();

	RenderTarget(const RenderTarget&) = delete;
	RenderTarget(RenderTarget&&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;
	RenderTarget& operator=(RenderTarget&&) = delete;

public:
	// レンダーターゲットに設定する
	void SetThisRenderTarget();

	void ChangeResourceState();

	// メインレンダーターゲットに変更(RenderTarget::SetThisTarget()を使ったら必ず呼ばなければならない)
	void SetMainRenderTarget();

	// レンダーターゲットに設定したResourceをShaderResourceとして使う
	void UseThisRenderTargetShaderResource();

	void CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU, UINT descHeapHandleUINT);
	void CreateRTV(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, UINT descHeapHandleUINT);

	UINT GetViewHandleUINT() const {
		return srvHeapHandleUint_;
	}

	Texture* GetTex() const {
		return tex_.get();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHeapHandle_;
	UINT srvHeapHandleUint_;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle_;
	UINT rtvHeapHandleUint_;

	std::unique_ptr<Texture> tex_;

	bool isResourceStateChange_;

	uint32_t width_, height_;
};