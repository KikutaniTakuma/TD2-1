#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector4.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"
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
	Line();
	~Line();

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

	ShaderResourceHeap heap;
	
	Shader shader;

	Pipeline* pipline;

	ConstBuffer<Mat4x4> wvpMat;
};