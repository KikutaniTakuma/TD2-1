#pragma once
#include "Engine/Engine.h"
#include <cassert>
#include <wrl.h>
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"

// ポインタをテンプレートパラメータに設定してはいけない
template<class T>
class StructuredBuffer {
	static_assert(!std::is_pointer<T>::value, "Do not use pointer types");

public:
	StructuredBuffer() = delete;

	inline StructuredBuffer(uint32_t instanceNum) noexcept :
		bufferResource(),
		srvDesc(),
		data(nullptr),
		isWright(true),
		isCreateView(false),
		range(),
		roootParamater(),
		instanceNum(instanceNum)
	{
		bufferResource = Engine::CreateBufferResuorce(sizeof(T) * instanceNum);
		srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = UINT(instanceNum);
		srvDesc.Buffer.StructureByteStride = sizeof(T);

		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}

		range = {};
		range.BaseShaderRegister = 1;
		range.NumDescriptors = 1;
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		roootParamater = {};
		roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		roootParamater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		roootParamater.DescriptorTable.pDescriptorRanges = &range;
		roootParamater.DescriptorTable.NumDescriptorRanges = 1;
	}

	inline ~StructuredBuffer() noexcept {
		if (bufferResource) {
			bufferResource->Release();
			bufferResource.Reset();
		}
	}

	inline StructuredBuffer(const StructuredBuffer& right) noexcept = delete;
	inline StructuredBuffer(StructuredBuffer&& right) noexcept = delete;

	inline StructuredBuffer<T>& operator=(const StructuredBuffer& right) = delete;
	inline StructuredBuffer<T>& operator=(StructuredBuffer&& right) = delete;

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

	T& operator[](uint32_t index) {
		assert(index < instanceNum);
		return data[index];
	}

	const T& operator[](uint32_t index) const {
		assert(index < instanceNum);
		return data[index];
	}

	void Resize(uint32_t indexNum) {
		assert(0 < indexNum);
		if (indexNum < 1) {
			ErrorCheck::GetInstance()->ErrorTextBox("Resize : Index must be greater than or equal to 1 ->" + std::to_string(indexNum), "StructuredBuffer");
			return;
		}

		OffWright();

		if (bufferResource) {
			bufferResource->Release();
			bufferResource.Reset();
		}

		instanceNum = indexNum;
		
		bufferResource = Engine::CreateBufferResuorce(sizeof(T) * instanceNum);
		srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = UINT(instanceNum);
		srvDesc.Buffer.StructureByteStride = sizeof(T);

		OnWright();
	}

	uint32_t Size() const {
		return instanceNum;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		return roootParamater;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) noexcept {
		Engine::GetDevice()->CreateShaderResourceView(bufferResource.Get(), &srvDesc, descriptorHandle);
		isCreateView = true;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	D3D12_DESCRIPTOR_RANGE range;
	D3D12_ROOT_PARAMETER roootParamater;

	T* data;

	bool isWright;

	bool isCreateView;


	uint32_t instanceNum;
};