#pragma once
#include "Engine/ConstBuffer/ConstBuffer.h"

class RenderTarget {
public:
	// ディスクリプタの大きさはデフォルトコンストラクタで4
	RenderTarget();
	RenderTarget(uint32_t width, uint32_t height);
	~RenderTarget();

public:
	// レンダーターゲットに設定する
	void SetThisRenderTarget();

	void ChangeResourceState();

	// メインレンダーターゲットに変更(RenderTarget::SetThisTarget()を使ったら必ず呼ばなければならない)
	void SetMainRenderTarget();

	// レンダーターゲットに設定したResourceをShaderResourceとして使う
	void UseThisRenderTargetShaderResource();

	void CreateView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHeapHandleGPU);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTVHeap;

	D3D12_GPU_DESCRIPTOR_HANDLE srvHeapHandle;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	bool isResourceStateChange;

	uint32_t width, height;
};