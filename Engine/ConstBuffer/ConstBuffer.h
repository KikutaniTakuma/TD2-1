#pragma once
#include "Engine/EngineParts/Direct3D/Direct3D.h"
#include "Engine/EngineParts/Direct12/Direct12.h"
#include <cassert>
#include <wrl.h>
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <type_traits>

template<class T>
concept IsNotPtrCB = !std::is_pointer_v<T>;

// ポインタをテンプレートパラメータに設定してはいけない
template<IsNotPtrCB T>
class ConstBuffer {
public:
	inline ConstBuffer() noexcept:
		bufferResource(),
		cbvDesc(),
		data(nullptr),
		isWright(true),
		isCreateView(false),
		roootParamater(),
		shaderVisibility(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister(0)
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		bufferResource = Direct3D::GetInstance()->CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
		cbvDesc.BufferLocation = bufferResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = UINT(bufferResource->GetDesc().Width);

		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}
		roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	}

	inline ~ConstBuffer() noexcept {
		if(bufferResource){
			bufferResource->Release();
			bufferResource.Reset();
		}
	}

	inline ConstBuffer(const ConstBuffer&) noexcept = delete;
	inline ConstBuffer(ConstBuffer&&) noexcept = delete;

	inline ConstBuffer<T>& operator=(const ConstBuffer&) = delete;
	inline ConstBuffer<T>& operator=(ConstBuffer&&) = delete;

public:
	void OnWright() noexcept {
		if (!isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
			isWright = !isWright;
		}
	}

	void OffWright() noexcept {
		if (isWright) {
			bufferResource->Unmap(0, nullptr);
			isWright = !isWright;
		}
	}

	T& operator*() const noexcept {
		return *data;
	}

	T* operator->() const noexcept {
		return data;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		roootParamater.ShaderVisibility = shaderVisibility;
		roootParamater.Descriptor.ShaderRegister = shaderRegister;
		return roootParamater;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHandleGPU, UINT dsecIndex) noexcept {
		static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
		device->CreateConstantBufferView(&cbvDesc, descHandle);
		descriptorHandle = descHandleGPU;
		dsecIndex_ = dsecIndex;
		isCreateView = true;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetViewHandle() const noexcept {
		return descriptorHandle;
	}

	UINT GetViewHandleUINT() const noexcept {
		return dsecIndex_;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	T* data;

	bool isWright;

	bool isCreateView;

	D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle;

	UINT dsecIndex_;

	D3D12_ROOT_PARAMETER roootParamater;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility;
	UINT shaderRegister;
};