#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Engine/Core/DirectXSwapChain/DirectXSwapChain.h"
#include <wrl.h>
#include "Utils/Cocepts/Cocepts.h"

/// <summary>
/// 定数バッファ
/// </summary>
template<Lamb::IsNotPtr T>
class ConstBuffer {
public:
	inline ConstBuffer() noexcept:
		bufferResource_(),
		cbvDesc_(),
		data_(nullptr),
		isWright_(true),
		isCreateView_(false),
		roootParamater_(),
		shaderVisibility_(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister_(0)
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
		cbvDesc_.BufferLocation = bufferResource_->GetGPUVirtualAddress();
		cbvDesc_.SizeInBytes = UINT(bufferResource_->GetDesc().Width);

		if (isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}
		roootParamater_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	}

	inline ~ConstBuffer() noexcept {
		if(bufferResource_){
			bufferResource_->Release();
			bufferResource_.Reset();
		}
	}

	inline ConstBuffer(const ConstBuffer& right) noexcept :
		ConstBuffer{}
	{
		*this = right;
	}
	inline ConstBuffer(ConstBuffer&&) noexcept = delete;

	inline ConstBuffer<T>& operator=(const ConstBuffer& right) {
		OnWright();

		*(*this) = *right;

		return *this;
	}
	inline ConstBuffer<T>& operator=(ConstBuffer&&) = delete;

public:
	void OnWright() noexcept {
		if (!isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			isWright_ = !isWright_;
		}
	}

	void OffWright() noexcept {
		if (isWright_) {
			bufferResource_->Unmap(0, nullptr);
			isWright_ = !isWright_;
		}
	}

	T& operator*() const noexcept {
		return *data_;
	}

	T* operator->() const noexcept {
		return data_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		roootParamater_.ShaderVisibility = shaderVisibility_;
		roootParamater_.Descriptor.ShaderRegister = shaderRegister_;
		return roootParamater_;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHandleGPU, UINT dsecIndex) noexcept {
		static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateConstantBufferView(&cbvDesc_, descHandle);
		descriptorHandle_ = descHandleGPU;
		dsecIndex_ = dsecIndex;
		isCreateView_ = true;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetViewHandle() const noexcept {
		return descriptorHandle_;
	}

	UINT GetViewHandleUINT() const noexcept {
		return dsecIndex_;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource_;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc_;

	T* data_;

	bool isWright_;

	bool isCreateView_;

	D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle_;

	UINT dsecIndex_;

	D3D12_ROOT_PARAMETER roootParamater_;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility_;
	UINT shaderRegister_;
};