#include "ShaderManager.h"
#include "Engine/Engine.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include <format>
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Utils/Log/Log.h"

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager::ShaderManager() {
	vertexShader.reserve(0);
	hullShader.reserve(0);
	domainShader.reserve(0);
	geometoryShader.reserve(0);
	pixelShader.reserve(0);

	// dxcCompilerを初期化
	dxcUtils = nullptr;
	dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("DxcCreateInstance() dxcUtils failed","ShaderManager");
	}

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("DxcCreateInstance() dxcCompiler failed", "ShaderManager");
	}

	includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(includeHandler.GetAddressOf());
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("CreateDefaultIncludeHandler() failed", "ShaderManager");
	}
}

ShaderManager::~ShaderManager() {
}

void ShaderManager::Initialize() {
	instance = new ShaderManager();
	assert(instance);
	if (!instance) {
		ErrorCheck::GetInstance()->ErrorTextBox("Instance failed", "ShaderManager");
	}
}

void ShaderManager::Finalize() {
	delete instance;
	instance = nullptr;
}

IDxcBlob* ShaderManager::CompilerShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile)
{
	// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Log::AddLog(ConvertString(std::format(L"Begin CompilerShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Shader Load failed", "ShaderManager");
	}
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	// 2. Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od", // 最適化を外しておく
		L"-Zpr" // メモリレイアウトを優先
	};
	// 実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer, // 読みこんだファイル
		arguments,           // コンパイルオプション
		_countof(arguments), // コンパイルオプションの数
		includeHandler.Get(),      // includeが含まれた諸々
		IID_PPV_ARGS(shaderResult.GetAddressOf()) // コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Danger!! Cannot use ""dxc""", "ShaderManager");
	}

	// 3. 警告・エラーが出てないか確認する
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		//Log::AddLog(shaderError->GetStringPointer());
		ErrorCheck::GetInstance()->ErrorTextBox(shaderError->GetStringPointer(), "ShaderManager");
		// 警告・エラーダメゼッタイ
		assert(false);
	}

	// 4. Compileを受け取って返す
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Log::AddLog(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

	// 実行用バイナリをリターン
	return shaderBlob;
}

IDxcBlob* ShaderManager::LoadVertexShader(const std::string& fileName) {
	if (instance->vertexShader.empty()) {
		IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"vs_6_0");
		assert(shader);
		instance->vertexShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance->vertexShader.find(fileName);
		if (itr == instance->vertexShader.end()) {
			IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"vs_6_0");
			assert(shader);
			instance->vertexShader.insert(std::make_pair(fileName, shader));
		}
	}
	return instance->vertexShader[fileName].Get();
}
IDxcBlob* ShaderManager::LoadHullShader(const std::string& fileName) {
	if (instance->hullShader.empty()) {
		IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"hs_6_0");
		assert(shader);
		instance->hullShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance->hullShader.find(fileName);
		if (itr == instance->hullShader.end()) {
			IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"hs_6_0");
			assert(shader);
			instance->hullShader.insert(std::make_pair(fileName, shader));
		}
	}

	return instance->hullShader[fileName].Get();
}
IDxcBlob* ShaderManager::LoadDomainShader(const std::string& fileName) {
	if (instance->domainShader.empty()) {
		IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"ds_6_0");
		assert(shader);
		instance->domainShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance->domainShader.find(fileName);
		if (itr == instance->domainShader.end()) {
			IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"ds_6_0");
			assert(shader);
			instance->domainShader.insert(std::make_pair(fileName, shader));
		}
	}
	return instance->domainShader[fileName].Get();
}
IDxcBlob* ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	if (instance->geometoryShader.empty()) {
		IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"gs_6_0");
		assert(shader);
		instance->geometoryShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance->geometoryShader.find(fileName);
		if (itr == instance->geometoryShader.end()) {
			IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"gs_6_0");
			assert(shader);
			instance->geometoryShader.insert(std::make_pair(fileName, shader));
		}
	}
	return instance->geometoryShader[fileName].Get();
}
IDxcBlob* ShaderManager::LoadPixelShader(const std::string& fileName) {
	if (instance->pixelShader.empty()) {
		IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"ps_6_0");
		assert(shader);
		instance->pixelShader.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance->pixelShader.find(fileName);
		if (itr == instance->pixelShader.end()) {
			IDxcBlob* shader = instance->CompilerShader(ConvertString(fileName), L"ps_6_0");
			assert(shader);
			instance->pixelShader.insert(std::make_pair(fileName, shader));
		}
	}
	return instance->pixelShader[fileName].Get();
}