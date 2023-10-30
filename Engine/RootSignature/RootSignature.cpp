#include "RootSignature.h"
#include <cassert>
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/Engine.h"
#include "Engine/EngineParts/Direct3D/Direct3D.h"

RootSignature::RootSignature():
	rootSignature{},
	isTexture(false)
{
	rootParamater = {};
}
RootSignature::RootSignature(const RootSignature& right) {
	*this = right;
}
RootSignature::RootSignature(RootSignature&& right) noexcept {
	*this = std::move(right);
}
RootSignature& RootSignature::operator=(const RootSignature& right) {
	rootSignature = right.rootSignature;
	rootParamater = right.rootParamater;
	isTexture = right.isTexture;

	return *this;
}
RootSignature& RootSignature::operator=(RootSignature&& right) noexcept {
	rootSignature = std::move(right.rootSignature);
	rootParamater = std::move(right.rootParamater);
	isTexture = std::move(right.isTexture);

	return *this;
}

bool RootSignature::operator==(const RootSignature& right) const {
	if (rootParamater.size() != right.rootParamater.size()) {
		return false;
	}
	for (size_t i = 0; i < rootParamater.size(); i++) {
		if (rootParamater[i].first != right.rootParamater[i].first) {
			return false;
		}
	}
	return  isTexture == right.isTexture;
}
bool RootSignature::operator!=(const RootSignature& right) const {
	return !(*this == right);
}

void RootSignature::Create(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_) {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootParamater.clear();
	rootParamater.reserve(rootParamaterSize_);
	for (size_t i = 0; i < rootParamaterSize_; i++) {
		std::vector<D3D12_DESCRIPTOR_RANGE> ranges = {};
		if (rootParamater_[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			for (uint32_t j = 0; j < rootParamater_[i].DescriptorTable.NumDescriptorRanges;j++) {
				ranges.push_back(rootParamater_[i].DescriptorTable.pDescriptorRanges[j]);
			}
		}
		rootParamater.push_back({ rootParamater_[i], ranges });
	}

	std::vector<D3D12_ROOT_PARAMETER> params = {};
	params.reserve(rootParamater.size());
	for (auto& i : rootParamater) {
		if (i.first.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			i.first.DescriptorTable.pDescriptorRanges = i.second.data();
			i.first.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(i.second.size());
		}
		params.push_back(i.first);
	}

	descriptionRootSignature.pParameters = params.data();
	descriptionRootSignature.NumParameters = static_cast<UINT>(params.size());

	isTexture = isTexture_;

	D3D12_STATIC_SAMPLER_DESC staticSamplers{};
	staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers.ShaderRegister = 0;
	staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	descriptionRootSignature.pStaticSamplers = isTexture ? &staticSamplers : nullptr;
	descriptionRootSignature.NumStaticSamplers = isTexture ? 1u : 0u;

	// シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox(reinterpret_cast<char*>(errorBlob->GetBufferPointer()), "RootSignature");
		assert(false);
	}
	// バイナリをもとに生成
	if (rootSignature) {
		rootSignature.Reset();
	}
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Device::CreateRootSignature() failed", "RootSignature");
	}
	if (errorBlob) { errorBlob.Reset(); }
	signatureBlob.Reset();
}

bool operator==(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right) {
	bool isSameDescriptorTable = false;
	if (left.ParameterType != right.ParameterType || 
		left.ShaderVisibility != right.ShaderVisibility) {
		return false;
	}
	else if(left.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE){
		if (left.DescriptorTable.NumDescriptorRanges == right.DescriptorTable.NumDescriptorRanges) {
			for (uint32_t i = 0; i < left.DescriptorTable.NumDescriptorRanges; i++) {
				isSameDescriptorTable =
					left.DescriptorTable.pDescriptorRanges[i].BaseShaderRegister == right.DescriptorTable.pDescriptorRanges[i].BaseShaderRegister
					&& left.DescriptorTable.pDescriptorRanges[i].NumDescriptors == right.DescriptorTable.pDescriptorRanges[i].NumDescriptors
					&& left.DescriptorTable.pDescriptorRanges[i].OffsetInDescriptorsFromTableStart == right.DescriptorTable.pDescriptorRanges[i].OffsetInDescriptorsFromTableStart
					&& left.DescriptorTable.pDescriptorRanges[i].RangeType == right.DescriptorTable.pDescriptorRanges[i].RangeType
					&& left.DescriptorTable.pDescriptorRanges[i].RegisterSpace == right.DescriptorTable.pDescriptorRanges[i].RegisterSpace;

				if (!isSameDescriptorTable) {
					return false;
				}
			}
		}
		else {
			return false;
		}
	}
	else if (left.ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV) {
		if (left.Descriptor.RegisterSpace == right.Descriptor.RegisterSpace&&
			left.Descriptor.ShaderRegister == right.Descriptor.ShaderRegister
			) {

			return true;
		}
	}
	else {
		return false;
	}

	return true;
}

bool operator!=(const D3D12_ROOT_PARAMETER& left, const D3D12_ROOT_PARAMETER& right) {
	return !(left == right);
}

bool RootSignature::IsSame(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_) const {
	if (rootParamater.size() != rootParamaterSize_) {
		return false;
	}
	for (size_t i = 0; i < rootParamater.size();i++) {
		if (rootParamater[i].first != rootParamater_[i]) {
			return false;
		}
	}
	return isTexture == isTexture_;
}