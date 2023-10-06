#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl.h>
#include <string>
#include <unordered_map>

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include "Engine/ShaderResource/ShaderResourceHeap.h"

/// <summary>
/// 基本的にポインタ型で使う
/// </summary>
class Mesh {
private:
	struct VertData {
		Vector4 position;
		Vector3 normal;
		Vector2 uv;
	};
	struct IndexData {
		uint32_t vertNum;
		uint32_t uvNum;
		uint32_t normalNum;

		inline bool operator==(const IndexData& right) {
			return vertNum == right.vertNum
				&& uvNum == right.uvNum
				&& normalNum == right.normalNum;
		}
		inline bool operator!=(const IndexData& right) {
			return !(*this == right);
		}
	};

	struct MeshData{
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		// 頂点バッファマップ
		VertData* vertexMap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW indexView{};
		// インデックスバッファマップ
		uint32_t* indexMap = nullptr;

		// 頂点数
		uint32_t vertNum = 0;
	};
	
public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	~Mesh();

public:
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) = delete;

public:
	void LoadObj(const std::string& objfileName);

	void LoadMtl(const std::string& fileName);

	void Use();

private:
	std::unordered_map<std::string, MeshData> meshs_;

	std::unordered_map<std::string, ShaderResourceHeap> SRVHeap_;

	std::unordered_map<std::string, Texture*> texs_;

	bool isLoad_;
};