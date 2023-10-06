#pragma once
#include <Windows.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <unordered_map>
#include <string>
#include <wrl.h>

struct Shader {
	IDxcBlob* vertex = nullptr;
	IDxcBlob* hull = nullptr;
	IDxcBlob* domain = nullptr;
	IDxcBlob* geometory = nullptr;
	IDxcBlob* pixel = nullptr;
};

class ShaderManager {
private:
	ShaderManager();
	~ShaderManager();

public:
	static void Initialize();

	static void Finalize();

	static inline ShaderManager* GetInstance() {
		return instance;
	}

public:
	static IDxcBlob* LoadVertexShader(const std::string& fileName);
	static IDxcBlob* LoadHullShader(const std::string& fileName);
	static IDxcBlob* LoadDomainShader(const std::string& fileName);
	static IDxcBlob* LoadGeometoryShader(const std::string& fileName);
	static IDxcBlob* LoadPixelShader(const std::string& fileName);

private:
	static ShaderManager* instance;

private:
	IDxcBlob* CompilerShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile);

private:
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler;

private:
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> vertexShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> hullShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> domainShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> geometoryShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> pixelShader;
};