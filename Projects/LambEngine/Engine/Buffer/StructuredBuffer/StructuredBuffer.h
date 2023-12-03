#pragma once
#include "Engine/Core/DirectXDevice/DirectXDevice.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Utils/Cocepts/Cocepts.h"
#include <cassert>

/// <summary>
/// ストラクチャードバッファ
/// </summary>
/// <typeparam name="T">ポインタ以外をサポート</typeparam>
template<Lamb::IsNotPtr T>
class StructuredBuffer {
public:
	StructuredBuffer() noexcept :
		bufferResource_(),
		srvDesc_(),
		data_(nullptr),
		isWright_(true),
		isCreateView_(false),
		range_(),
		roootParamater_(),
		instanceNum_(1u)
	{
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(T) * 1u);
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = UINT(instanceNum_);
		srvDesc_.Buffer.StructureByteStride = sizeof(T);

		if (isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}

		range_ = {};
		range_.BaseShaderRegister = 1;
		range_.NumDescriptors = 1;
		range_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		roootParamater_ = {};
		roootParamater_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		roootParamater_.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		roootParamater_.DescriptorTable.pDescriptorRanges = &range_;
		roootParamater_.DescriptorTable.NumDescriptorRanges = 1;
	}

	inline StructuredBuffer(uint32_t instanceNum) noexcept :
		bufferResource_(),
		srvDesc_(),
		data_(nullptr),
		isWright_(true),
		isCreateView_(false),
		range_(),
		roootParamater_(),
		instanceNum_(instanceNum)
	{
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(T) * instanceNum);
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = UINT(instanceNum);
		srvDesc_.Buffer.StructureByteStride = sizeof(T);

		if (isWright_) {
			bufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}

		range_ = {};
		range_.BaseShaderRegister = 1;
		range_.NumDescriptors = 1;
		range_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		roootParamater_ = {};
		roootParamater_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		roootParamater_.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		roootParamater_.DescriptorTable.pDescriptorRanges = &range_;
		roootParamater_.DescriptorTable.NumDescriptorRanges = 1;
	}

	inline ~StructuredBuffer() noexcept {
		if (bufferResource_) {
			bufferResource_->Release();
			bufferResource_.Reset();
		}
	}

	inline StructuredBuffer(const StructuredBuffer& right) noexcept:
		StructuredBuffer{ right.Size() }
	{
		*this = right;
	}

	template<Lamb::IsContainer Container>
	inline StructuredBuffer(const Container& right) noexcept :
		StructuredBuffer{ right.size() }
	{
		*this = right;
	}
	inline StructuredBuffer(StructuredBuffer&&) = delete;

	inline StructuredBuffer<T>& operator=(const StructuredBuffer& right) noexcept {
		Resize(right.Size());

		OnWright();

		for (uint32_t i = 0u; i < instanceNum_; i++) {
			data_[i] = right[i];
		}

		return *this;
	}
	inline StructuredBuffer<T>& operator=(StructuredBuffer&&) = delete;

	template<Lamb::IsContainer Container>
	inline StructuredBuffer<T>& operator=(const Container& right) noexcept {
		Resize(right.size());

		OnWright();

		uint32_t index = 0u;
		for (const auto& i : right) {
			data_[index] = i;
			index++;
		}

		return *this;
	}

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

	template<std::integral IsInt>
	T& operator[](IsInt index) {
		assert(static_cast<uint32_t>(index) < instanceNum_);
		return data_[index];
	}

	template<std::integral IsInt>
	const T& operator[](IsInt index) const {
		assert(static_cast<uint32_t>(index) < instanceNum_);
		return data_[index];
	}

	void Resize(uint32_t indexNum) {
		assert(0 < indexNum);
		if (indexNum == instanceNum_) {
			return;
		}

		if (indexNum < 1) {
			Log::ErrorLog("Index must be greater than or equal to 1 ->" + std::to_string(indexNum), "Resize()","StructuredBuffer");
			return;
		}

		std::vector<T> tmp;
		for (uint32_t i = 0; i < instanceNum_; i++) {
			tmp.push_back(data_[i]);
		}

		OffWright();

		if (bufferResource_) {
			bufferResource_->Release();
			bufferResource_.Reset();
		}

		instanceNum_ = indexNum;
		
		bufferResource_ = DirectXDevice::GetInstance()->CreateBufferResuorce(sizeof(T) * instanceNum_);
		srvDesc_ = {};
		srvDesc_.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc_.Buffer.FirstElement = 0;
		srvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc_.Buffer.NumElements = UINT(instanceNum_);
		srvDesc_.Buffer.StructureByteStride = sizeof(T);

		CrerateView(descriptorHandleCPU_, descriptorHandleGPU_, dsecIndex_);

		OnWright();

		for (uint32_t i = 0; i < instanceNum_; i++) {
			if (tmp.size() <= i) {
				break;
			}
			data_[i] = tmp[i];
		}
	}

	uint32_t Size() const {
		return instanceNum_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource_->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		return roootParamater_;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descHandle, D3D12_GPU_DESCRIPTOR_HANDLE descHandleGPU, UINT dsecIndex) noexcept {
		static ID3D12Device* device = DirectXDevice::GetInstance()->GetDevice();
		device->CreateShaderResourceView(bufferResource_.Get(), &srvDesc_, descHandle);
		descriptorHandleCPU_ = descHandle;
		isCreateView_ = true;
		descriptorHandleGPU_ = descHandleGPU;
		dsecIndex_ = dsecIndex;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetViewHandle() const noexcept {
		return descriptorHandleGPU_;
	}

	UINT GetViewHandleUINT() const noexcept {
		return dsecIndex_;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource_;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_;

	D3D12_DESCRIPTOR_RANGE range_;
	D3D12_ROOT_PARAMETER roootParamater_;

	T* data_;

	bool isWright_;

	bool isCreateView_;


	uint32_t instanceNum_;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandleGPU_;

	UINT dsecIndex_;
};