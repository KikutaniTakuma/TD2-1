#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <vector>
#include <cstdint>

class Direct12 {
private:
	Direct12();
	Direct12(const Direct12&) = delete;
	Direct12(Direct12&&) = delete;
	~Direct12();

public:
	Direct12& operator=(const Direct12&) = delete;
	Direct12& operator=(Direct12&&) = delete;

public:
	static Direct12* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static Direct12* instance_;

public:
	/// <summary>
	/// コマンドリストの実行を待つ
	/// </summary>
	void WaitForFinishCommnadlist();

	/// <summary>
	/// ビューポートセット
	/// </summary>
	/// <param name="width">基本的にバックバッファと同じにする</param>
	/// <param name="height">基本的にバックバッファと同じにする</param>
	void SetViewPort(uint32_t width, uint32_t height);

	/// <summary>
	/// バリア
	/// </summary>
	/// <param name="resource">リソースバリアを貼るリソース</param>
	/// <param name="before">今の状態</param>
	/// <param name="after">遷移後の状態</param>
	/// <param name="subResource">サブリソース</param>
	void Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, UINT subResource = 0u);

	/// <summary>
	/// メインレンダーターゲットにセットする
	/// </summary>
	void SetMainRenderTarget();

	void ClearBackBuffer();

	/// <summary>
	/// バックバッファの状態を変更する
	/// </summary>
	void ChangeBackBufferState();

	/// <summary>
	/// コマンドリストを確定させる
	/// </summary>
	void CloseCommandlist();

	/// <summary>
	/// コマンドリストを実行
	/// </summary>
	void ExecuteCommandLists();

	/// <summary>
	/// スワップチェインに画面の交換を行う
	/// </summary>
	void SwapChainPresent();

	/// <summary>
	/// コマンドリストのリセット(また使えるようにする)
	/// </summary>
	void ResetCommandlist();

public:
	ID3D12GraphicsCommandList* GetCommandList() const {
		return commandList_.Get();
	}

	ID3D12CommandQueue* GetCommandQueue() const {
		return commandQueue_.Get();
	}

	IDXGISwapChain4* GetSwapChain() const {
		return swapChain_.Get();
	}

	inline D3D12_DESCRIPTOR_HEAP_DESC GetMainRTVDesc() const {
		return rtvDescriptorHeap_->GetDesc();
	}

	inline D3D12_RESOURCE_DESC GetSwapchainBufferDesc() const {
		return swapChainResource_[0]->GetDesc();
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GetMainRendertTargetHandle() const {
		UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
		return rtvHandles_[backBufferIndex];
	}

	inline bool GetIsCloseCommandList() const {
		return isCommandListClose_;
	}


private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	bool isCommandListClose_;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResource_;
	bool isRenderState_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles_;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint64_t fenceVal_;
	HANDLE fenceEvent_;
};