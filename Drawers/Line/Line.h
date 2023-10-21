#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector4.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/ShaderManager/ShaderManager.h"
class Pipeline;

class Line {
private:
	struct VertexData {
		Vector4 pos;
		Vector4 color;
	};
private:
	static constexpr uint16_t kVertexNum = 2u;

public:
	static void Initialize();

private:
	static Shader shader;

	static Pipeline* pipline;

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
	Vector3 start;
	Vector3 end;

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexView;
	// 頂点バッファマップ
	VertexData* vertexMap;

	class ShaderResourceHeap* heap;

	ConstBuffer<Mat4x4> wvpMat;
};