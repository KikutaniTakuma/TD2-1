#include "Utils/ConvertString/ConvertString.h"
#include "Engine/Engine.h"
#include "Engine/EngineParts/Direct3D/Direct3D.h"
#include "Engine/EngineParts/Direct12/Direct12.h"
#include <cassert>
#include <iostream>
#include <filesystem>
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "TextureManager/TextureManager.h"

Texture::Texture():
	textureResouce(nullptr),
	intermediateResource(nullptr),
	srvDesc(),
	isLoad(false),
	threadLoadFlg(false),
	size(),
	fileName(),
	heapPos(0)
{}

Texture::~Texture() {
	Unload();
}


Texture::Texture(Texture&& tex) noexcept {
	*this = std::move(tex);
}
Texture& Texture::operator=(Texture&& tex) noexcept {
	this->textureResouce = std::move(tex.textureResouce);
	this->intermediateResource = std::move(tex.intermediateResource);
	this->srvDesc = tex.srvDesc;
	isLoad = tex.isLoad;
	fileName = tex.fileName;

	return *this;
}

void Texture::Load(const std::string& filePath) {
	if (!isLoad && !threadLoadFlg) {
		this->fileName = filePath;

		DirectX::ScratchImage mipImages = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		size = { static_cast<float>(metadata.width),static_cast<float>(metadata.height) };
		textureResouce = CreateTextureResource(metadata);

		if (textureResouce && !Direct12::GetInstance()->GetIsCloseCommandList()) {
			intermediateResource = UploadTextureData(textureResouce.Get(), mipImages);
		}
		else {
			return;
		}

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		// load済み
		isLoad = true;
	}
}

void Texture::Load(const std::string& filePath, ID3D12GraphicsCommandList* commandList) {
	if (!isLoad && !threadLoadFlg) {
		this->fileName = filePath;

		DirectX::ScratchImage mipImages = LoadTexture(filePath);
		const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
		size = { static_cast<float>(metadata.width),static_cast<float>(metadata.height) };
		textureResouce = CreateTextureResource(metadata);

		if (textureResouce) {
			intermediateResource = UploadTextureData(textureResouce.Get(), mipImages, commandList);
		}
		else {
			return;
		}

		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

		// load済み
		isLoad = true;
	}
}

void Texture::Unload() {
	if (isLoad) {
		srvDesc = {};

		if (intermediateResource) {
			intermediateResource->Release();
			intermediateResource.Reset();
		}
		if (textureResouce) {
			textureResouce->Release();
			textureResouce.Reset();
		}

		// Unload済み
		isLoad = false;
	}
}


DirectX::ScratchImage Texture::LoadTexture(const std::string& filePath) {
	if (!std::filesystem::exists(std::filesystem::path(filePath))) {
		ErrorCheck::GetInstance()->ErrorTextBox("LoadTexture() : Failed : This file is not exist -> " + filePath, "Texture");
		return DirectX::ScratchImage();
	}

	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("LoadTexture() : DirectX::LoadFromWICFile() failed", "Texture");
		return DirectX::ScratchImage();
	}

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("LoadTexture() : DirectX::GenerateMipMaps failed", "Texture");
		return DirectX::ScratchImage();
	}


	// ミップマップ付きのデータを返す
	return mipImages;
}

ID3D12Resource* Texture::CreateTextureResource(const DirectX::TexMetadata& metaData) {
	ID3D12Device* device = Direct3D::GetInstance()->GetDevice();

	if (metaData.width == 0 || metaData.height == 0) {
		return nullptr;
	}

	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metaData.width);
	resourceDesc.Height = UINT(metaData.height);
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);
	resourceDesc.Format = metaData.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	// 利用するHeapの設定。非常に特殊な運用
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resouceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	if (hr != S_OK) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateTextureResource() failed", "Texture");
		return nullptr;
	}
	return resource;
}

[[nodiscard]]
ID3D12Resource* Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* resource = Direct3D::GetInstance()->CreateBufferResuorce(intermediateSize);
	UpdateSubresources(Direct12::GetInstance()->GetCommandList(), texture, resource, 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResouceStateを変更する
	Direct12::GetInstance()->Barrier(
		texture,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
	);
	
	return resource;
}

[[nodiscard]]
ID3D12Resource* Texture::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12GraphicsCommandList* commandList) {
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* resource = Direct3D::GetInstance()->CreateBufferResuorce(intermediateSize);
	UpdateSubresources(commandList, texture, resource, 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResouceStateを変更する
	
	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース
	barrier.Transition.pResource = texture;
	// subResourceの設定
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// 遷移前(現在)のResouceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	// 遷移後のResouceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	// TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	return resource;
}



void Texture::CreateSRVView(D3D12_CPU_DESCRIPTOR_HANDLE descHeapHandle) {
	static ID3D12Device* device = Direct3D::GetInstance()->GetDevice();
	device->CreateShaderResourceView(textureResouce.Get(), &srvDesc, descHeapHandle);
}


void Texture::ReleaseIntermediateResource() {
	if (intermediateResource) {
		intermediateResource->Release();
		intermediateResource.Reset();
	}
}

void Texture::Use(UINT rootParamator) {
	static TextureManager* textureManager = TextureManager::GetInstance();
	assert(textureManager);
	textureManager->Use(heapPos, rootParamator);
}