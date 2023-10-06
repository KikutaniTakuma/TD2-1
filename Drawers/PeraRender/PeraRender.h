#pragma once
#include <string>
#include <array>

#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/RenderTarget/RenderTarget.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "Engine/ShaderManager/ShaderManager.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Vector2.h"

class PeraRender {
public:
	struct PeraVertexData {
		Vector3 position;
		Vector2 uv;
	};

	struct Wipe {
		Vector2 center;
		float wipeSize;
	};

public:
	PeraRender();
	PeraRender(uint32_t width_, uint32_t height_);
	~PeraRender();

	PeraRender& operator=(const PeraRender&) = default;

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void PreDraw();

	void Draw(Pipeline::Blend blend, PeraRender* pera = nullptr);

private:
	RenderTarget render;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraVertexResource = nullptr;
	Shader shader;

	std::array<Pipeline*, 3> piplines;
};