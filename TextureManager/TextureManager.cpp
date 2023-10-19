#include "TextureManager.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/Engine.h"
#include <cassert>
#include "Engine/ShaderResource/ShaderResourceHeap.h"

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {
	return instance;
}

void TextureManager::Initialize() {
	instance = new TextureManager();
	assert(instance);
	instance->LoadTexture("./Resources/white2x2.png");
}

void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}

TextureManager::TextureManager() :
	textures(),
	thisFrameLoadFlg(false),
	threadTextureBuff(),
	load(),
	isThreadFinish(false),
	fence(),
	fenceVal(0),
	fenceEvent(nullptr),
	srvHeap(nullptr)
{
	// コマンドキューを作成
	commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = Engine::GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateCommandQueue() Failed", "TextureManager");
		return;
	}

	// コマンドアロケータを生成する
	commandAllocator = nullptr;
	hr = Engine::GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateCommandAllocator() Failed", "TextureManager");
		return;
	}

	// コマンドリストを作成する
	commandList = nullptr;
	hr = Engine::GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateCommandList() Failed", "TextureManager");
		return;
	}

	// 初期値0でFenceを作る
	fence = nullptr;
	fenceVal = 0;
	hr = Engine::GetDevice()->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateFence() Failed", "TextureManager");
		return;
	}

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent = nullptr;
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
	if (!(fenceEvent != nullptr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("TextureManager() : CreateEvent() Failed", "TextureManager");
		return;
	}

	srvHeap = ShaderResourceHeap::GetInstance();
}

TextureManager::~TextureManager() {
	if (load.joinable()) {
		load.join();
	}
	CloseHandle(fenceEvent);
	textures.clear();
}


Texture* TextureManager::LoadTexture(const std::string& fileName) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList.Get());

		/// コマンドリスト
		commandList->Close();
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		// Fenceの値を更新
		fenceVal++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence.Get(), fenceVal);

		// Fenceの値が指定したSigna値にたどり着いているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceVal) {
			// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence->SetEventOnCompletion(fenceVal, fenceEvent);
			// イベントを待つ
			WaitForSingleObject(fenceEvent, INFINITE);
		}

		// 次フレーム用のコマンドリストを準備
		auto hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
		}
		hr = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
		}
		///

		if (!tex->isLoad) {
			return nullptr;
		}

		tex->heapPos = srvHeap->CreateTxtureView(tex.get());
		
		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName, commandList.Get());

			/// コマンドリスト
			commandList->Close();
			ID3D12CommandList* commandLists[] = { commandList.Get() };
			commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
			// Fenceの値を更新
			fenceVal++;
			// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			commandQueue->Signal(fence.Get(), fenceVal);

			// Fenceの値が指定したSigna値にたどり着いているか確認する
			// GetCompletedValueの初期値はFence作成時に渡した初期値
			if (fence->GetCompletedValue() < fenceVal) {
				// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
				fence->SetEventOnCompletion(fenceVal, fenceEvent);
				// イベントを待つ
				WaitForSingleObject(fenceEvent, INFINITE);
			}

			// 次フレーム用のコマンドリストを準備
			auto hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			if (!SUCCEEDED(hr)) {
				ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
			}
			hr = commandList->Reset(commandAllocator.Get(), nullptr);
			assert(SUCCEEDED(hr));
			if (!SUCCEEDED(hr)) {
				ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
			}
			///

			if (!tex->isLoad) {
				return nullptr;
			}

			tex->heapPos = srvHeap->CreateTxtureView(tex.get());

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}
	
	return textures[fileName].get();
}

Texture* TextureManager::LoadTexture(const std::string& fileName, ID3D12GraphicsCommandList* commandList_) {
	if (textures.empty()) {
		auto tex = std::make_unique<Texture>();
		tex->Load(fileName, commandList_);
		if (!tex->isLoad) {
			return nullptr;
		}

		tex->heapPos = srvHeap->CreateTxtureView(tex.get());

		textures.insert(std::make_pair(fileName, std::move(tex)));

		thisFrameLoadFlg = true;
	}
	else {
		auto itr = textures.find(fileName);
		if (itr == textures.end()) {
			auto tex = std::make_unique<Texture>();
			tex->Load(fileName, commandList_);
			if (!tex->isLoad) {
				return nullptr;
			}

			tex->heapPos = srvHeap->CreateTxtureView(tex.get());

			textures.insert(std::make_pair(fileName, std::move(tex)));

			thisFrameLoadFlg = true;
		}
	}

	return textures[fileName].get();
}

void TextureManager::LoadTexture(const std::string& fileName, Texture** texPtr) {
	if (load.joinable() && threadTextureBuff.empty()) {
		load.join();
	}
	
	// コンテナに追加
	threadTextureBuff.push({ fileName, texPtr });
}

void TextureManager::ThreadLoadTexture() {
	if (!load.joinable() && !threadTextureBuff.empty()) {
		auto loadProc = [this]() {
			while (!threadTextureBuff.empty()) {
				auto&& front = threadTextureBuff.front();
				*front.second = LoadTexture(front.first, commandList.Get());
				threadTextureBuff.pop();
			}

			isThreadFinish = true;
		};

		load = std::thread(loadProc);
	}
}

Texture* TextureManager::GetWhiteTex() {
	return instance->textures["./Resources/white2x2.png"].get();
}

void TextureManager::ReleaseIntermediateResource() {
	if (thisFrameLoadFlg && threadTextureBuff.empty()) {
		for (auto& i : textures) {
			i.second->ReleaseIntermediateResource();
		}

		thisFrameLoadFlg = false;
	}
}

void TextureManager::ResetCommandList() {
	if (isThreadFinish) {
		commandList->Close();
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
		// Fenceの値を更新
		fenceVal++;
		// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
		commandQueue->Signal(fence.Get(), fenceVal);

		// Fenceの値が指定したSigna値にたどり着いているか確認する
		// GetCompletedValueの初期値はFence作成時に渡した初期値
		if (fence->GetCompletedValue() < fenceVal) {
			// 指定したSignal値にたどり着いていないので、たどり着くまで待つようにイベントを設定する
			fence->SetEventOnCompletion(fenceVal, fenceEvent);
			// イベントを待つ
			WaitForSingleObject(fenceEvent, INFINITE);
		}

		load.join();

		// 次フレーム用のコマンドリストを準備
		auto hr = commandAllocator->Reset();
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("CommandAllocator->Reset() Failed", "Engine");
		}
		hr = commandList->Reset(commandAllocator.Get(), nullptr);
		assert(SUCCEEDED(hr));
		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("CommandList->Reset() Failed", "Engine");
		}

		isThreadFinish = false;
	}
}

void TextureManager::Use(int32_t texIndex, UINT rootParam) {
	auto* const mainComlist = Engine::GetCommandList();
	mainComlist->SetGraphicsRootDescriptorTable(
		rootParam, srvHeap->GetSrvGpuHeapHandle(texIndex));
}