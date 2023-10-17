#pragma once
#include "Engine/Engine.h"
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
		bufferResource = Engine::CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
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

	inline ConstBuffer(const ConstBuffer& right) noexcept :
		bufferResource(),
		cbvDesc(),
		data(nullptr),
		isWright(true),
		roootParamater(),
		shaderVisibility(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister(0)
	{
		*this = right;
	}

	inline ConstBuffer<T>& operator=(const ConstBuffer& right) {

		if (bufferResource) {
			bufferResource->Release();
			bufferResource.Reset();
		}
		bufferResource = Engine::CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
		cbvDesc.BufferLocation = bufferResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = UINT(bufferResource->GetDesc().Width);

		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}
		roootParamater = right.roootParamater;

		*data = *right.data;

		if (!isCreateView) {
			assert(!"created view");
			ErrorCheck::GetInstance()->ErrorTextBox("operator= Created view fail", "Const Buffer");
		}

		return *this;
	}

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

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) noexcept {
		Engine::GetDevice()->CreateConstantBufferView(&cbvDesc, descriptorHandle);
		isCreateView = true;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	T* data;

	bool isWright;

	bool isCreateView;

	D3D12_ROOT_PARAMETER roootParamater;
public:
	D3D12_SHADER_VISIBILITY shaderVisibility;
	UINT shaderRegister;
};