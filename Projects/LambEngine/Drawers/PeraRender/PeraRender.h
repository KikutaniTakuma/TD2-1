#pragma once
#include <string>
#include <array>

#include "Engine/Buffer/ConstBuffer/ConstBuffer.h"
#include "Engine/Graphics/RenderTarget/RenderTarget.h"
#include "Engine/Graphics/ShaderManager/ShaderManager.h"
#include "Engine/Graphics/PipelineManager/PipelineManager.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Mat4x4.h"

/// <summary>
/// ポストエフェクトの描画
/// </summary>
class PeraRender {
public:
	struct PeraVertexData {
		Vector3 position_;
		Vector2 uv_;
	};

	struct Wipe {
		Vector2 center_;
		float wipeSize_;
	};

public:
	PeraRender();
	PeraRender(uint32_t width, uint32_t height);
	~PeraRender();

	PeraRender(const PeraRender&) = delete;
	PeraRender(PeraRender&&) = delete;
	PeraRender& operator=(const PeraRender&) = delete;
	PeraRender& operator=(PeraRender&&) = delete;

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void Update();

	void PreDraw();

	void Draw(const Mat4x4& viewProjection, Pipeline::Blend blend, PeraRender* pera = nullptr);

	Texture* GetTex() const {
		return render_.GetTex();
	}

	void ChangeResourceState() {
		render_.ChangeResourceState();
	}

	void SetMainRenderTarget() {
		render_.SetMainRenderTarget();
	}

public:
	Vector3 pos_;
	Vector3 rotate_;
	Vector3 scale_;

	Vector2 uvPibot_;
	Vector2 uvSize_;

	std::array<Vector3, 4> worldPos_;

	uint32_t color_;

private:
	RenderTarget render_;

	ConstBuffer<Mat4x4> wvpMat_;
	ConstBuffer<Vector4> colorBuf_;

	bool isPreDraw_;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraVertexResource_ = nullptr;
	Shader shader_;

	D3D12_INDEX_BUFFER_VIEW indexView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	std::array<class Pipeline*, Pipeline::Blend::BlendTypeNum> piplines_;
};