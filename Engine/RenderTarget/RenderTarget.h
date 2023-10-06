#pragma once
#include "Engine/ConstBuffer/ConstBuffer.h"

class RenderTarget {
public:
	// ディスクリプタの大きさはデフォルトコンストラクタで4
	RenderTarget();
	RenderTarget(uint32_t width, uint32_t height);
	RenderTarget(uint16_t numDescriptor);
	RenderTarget(uint16_t numDescriptor, uint32_t width, uint32_t height);
	~RenderTarget();

public:
	// レンダーターゲットに設定する
	void SetThisRenderTarget();

	void ChangeResourceState();

	// メインレンダーターゲットに変更(RenderTarget::SetThisTarget()を使ったら必ず呼ばなければならない)
	void SetMainRenderTarget();

	// レンダーターゲットに設定したResourceをShaderResourceとして使う
	void UseThisRenderTargetShaderResource();

	template<class T>
	void CreateConstBufferView(ConstBuffer<T>& conBuf) {
		conBuf.CrerateView(srvHeapHandle);
		srvHeapHandle.ptr += Engine::GetIncrementSRVCBVUAVHeap();
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHeapHandle;

	bool isResourceStateChange;

	uint32_t width, height;
};