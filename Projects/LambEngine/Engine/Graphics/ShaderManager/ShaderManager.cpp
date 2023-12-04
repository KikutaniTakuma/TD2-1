#include "ShaderManager.h"
#include "Engine/Engine.h"
#include "Utils/ConvertString/ConvertString.h"
#include <cassert>
#include <format>
#include "Utils/ExecutionLog/ExecutionLog.h"
#pragma warning(disable: 6387)

ShaderManager* ShaderManager::instance_ = nullptr;

ShaderManager::ShaderManager() {
	vertexShader_.reserve(0);
	hullShader_.reserve(0);
	domainShader_.reserve(0);
	geometoryShader_.reserve(0);
	pixelShader_.reserve(0);

	// dxcCompilerを初期化
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("dxcUtils failed", "DxcCreateInstance()","ShaderManager");
	}

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler_.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("dxcCompiler failed", "DxcCreateInstance() ", "ShaderManager");
	}

	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf());
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("IDxcUtils failed", "CreateDefaultIncludeHandler()" "ShaderManager");
	}
}

ShaderManager::~ShaderManager() {
}

void ShaderManager::Initialize() {
	instance_ = new ShaderManager();
	assert(instance_);
}

void ShaderManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

IDxcBlob* ShaderManager::CompilerShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile)
{
	// 1. hlslファイルを読む
	// これからシェーダーをコンパイルする旨をログに出す
	Lamb::AddLog(ConvertString(std::format(L"Begin CompilerShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("Shader Load failed", "CompilerShader", "ShaderManager");
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
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer, // 読みこんだファイル
		arguments,           // コンパイルオプション
		_countof(arguments), // コンパイルオプションの数
		includeHandler_.Get(),      // includeが含まれた諸々
		IID_PPV_ARGS(shaderResult.GetAddressOf()) // コンパイル結果
	);

	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Lamb::ErrorLog("Danger!! Cannot use ""dxc""", "CompilerShader", "ShaderManager");
		return nullptr;
	}

	if (!shaderResult) {
		Lamb::ErrorLog("Create ShaderResult failed", "CompilerShader", "ShaderManager");
		return nullptr;
	}

	// 3. 警告・エラーが出てないか確認する
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		//Log::AddLog(shaderError->GetStringPointer());
		Lamb::ErrorLog(shaderError->GetStringPointer(), "CompilerShader", "ShaderManager");
		// 警告・エラーダメゼッタイ
		assert(false);
		return nullptr;
	}

	// 4. Compileを受け取って返す
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Lamb::AddLog(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

	// 実行用バイナリをリターン
	return shaderBlob;
}

IDxcBlob* ShaderManager::LoadVertexShader(const std::string& fileName) {
	if (instance_->vertexShader_.empty()) {
		IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"vs_6_0");
		assert(shader);
		instance_->vertexShader_.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance_->vertexShader_.find(fileName);
		if (itr == instance_->vertexShader_.end()) {
			IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"vs_6_0");
			assert(shader);
			instance_->vertexShader_.insert(std::make_pair(fileName, shader));
		}
	}
	return instance_->vertexShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadHullShader(const std::string& fileName) {
	if (instance_->hullShader_.empty()) {
		IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"hs_6_0");
		assert(shader);
		instance_->hullShader_.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance_->hullShader_.find(fileName);
		if (itr == instance_->hullShader_.end()) {
			IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"hs_6_0");
			assert(shader);
			instance_->hullShader_.insert(std::make_pair(fileName, shader));
		}
	}

	return instance_->hullShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadDomainShader(const std::string& fileName) {
	if (instance_->domainShader_.empty()) {
		IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"ds_6_0");
		assert(shader);
		instance_->domainShader_.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance_->domainShader_.find(fileName);
		if (itr == instance_->domainShader_.end()) {
			IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"ds_6_0");
			assert(shader);
			instance_->domainShader_.insert(std::make_pair(fileName, shader));
		}
	}
	return instance_->domainShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadGeometoryShader(const std::string& fileName) {
	if (instance_->geometoryShader_.empty()) {
		IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"gs_6_0");
		assert(shader);
		instance_->geometoryShader_.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance_->geometoryShader_.find(fileName);
		if (itr == instance_->geometoryShader_.end()) {
			IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"gs_6_0");
			assert(shader);
			instance_->geometoryShader_.insert(std::make_pair(fileName, shader));
		}
	}
	return instance_->geometoryShader_[fileName].Get();
}
IDxcBlob* ShaderManager::LoadPixelShader(const std::string& fileName) {
	if (instance_->pixelShader_.empty()) {
		IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"ps_6_0");
		assert(shader);
		instance_->pixelShader_.insert(std::make_pair(fileName, shader));
	}
	else {
		auto itr = instance_->pixelShader_.find(fileName);
		if (itr == instance_->pixelShader_.end()) {
			IDxcBlob* shader = instance_->CompilerShader(ConvertString(fileName), L"ps_6_0");
			assert(shader);
			instance_->pixelShader_.insert(std::make_pair(fileName, shader));
		}
	}
	return instance_->pixelShader_[fileName].Get();
}