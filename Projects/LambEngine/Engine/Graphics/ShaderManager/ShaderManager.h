#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <unordered_map>
#include <string>
#include <wrl.h>

/// <summary>
/// シェーダーをまとめる構造体
/// </summary>
struct Shader {
	IDxcBlob* vertex_ = nullptr;
	IDxcBlob* hull_ = nullptr;
	IDxcBlob* domain_ = nullptr;
	IDxcBlob* geometory_ = nullptr;
	IDxcBlob* pixel_ = nullptr;
};

/// <summary>
/// シェーダーを管理するクラス
/// </summary>
class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();

public:
	static void Initialize();

	static void Finalize();

	static inline ShaderManager* GetInstance() {
		return instance_;
	}

public:
	static IDxcBlob* LoadVertexShader(const std::string& fileName);
	static IDxcBlob* LoadHullShader(const std::string& fileName);
	static IDxcBlob* LoadDomainShader(const std::string& fileName);
	static IDxcBlob* LoadGeometoryShader(const std::string& fileName);
	static IDxcBlob* LoadPixelShader(const std::string& fileName);

private:
	static ShaderManager* instance_;

private:
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile);

private:
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

private:
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> vertexShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> hullShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> domainShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> geometoryShader_;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> pixelShader_;
};