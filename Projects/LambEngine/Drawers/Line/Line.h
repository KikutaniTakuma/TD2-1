#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector4.h"
#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/ShaderManager/ShaderManager.h"

/// <summary>
/// 線の描画
/// </summary>
class Line {
private:
	struct VertexData {
		Vector4 pos_;
		Vector4 color_;
	};
private:
	static constexpr uint16_t kVertexNum = 2u;

public:
	static void Initialize();

private:
	static Shader shader_;

	static class Pipeline* pipline_;

public:
	Line();
	Line(const Line& right);
	Line(Line&& right) noexcept;
	~Line();

	Line& operator=(const Line& right);
	Line& operator=(Line&& right)noexcept;

public:
	void Draw(const Mat4x4& viewProjection, uint32_t color);

public:
	Vector3 start_;
	Vector3 end_;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexView_;
	// 頂点バッファマップ
	VertexData* vertexMap_;

	class CbvSrvUavHeap* heap_;

	ConstBuffer<Mat4x4> wvpMat_;
};