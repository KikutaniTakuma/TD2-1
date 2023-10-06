#pragma once
#include "Engine/ShaderManager/ShaderManager.h"
class RootSignature;

class Pipeline {
public:
	/// <summary>
	/// フレンドクラス
	/// </summary>
	friend class PipelineManager;

/// <summary>
/// サブクラス
/// </summary>
public:
	enum Blend : uint16_t {
		None,
		Normal,
		Add,
		Sub,
		Mul,

		BlendTypeNum,
	};

	enum class CullMode {
		None = D3D12_CULL_MODE_NONE,
		Back = D3D12_CULL_MODE_BACK,
		Front = D3D12_CULL_MODE_FRONT,
	};
	enum class SolidState {
		Wireframe = D3D12_FILL_MODE_WIREFRAME,
		Solid = D3D12_FILL_MODE_SOLID,
	};

/// <summary>
/// コンストラクタ
/// </summary>
public:
	Pipeline();
	~Pipeline() = default;
	Pipeline(Pipeline&& right) noexcept;
	Pipeline(const Pipeline&) = delete;
/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline& operator=(Pipeline&& right) noexcept;

	bool operator==(const Pipeline& right) const;
	bool operator!=(const Pipeline& right) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void SetVertexInput(
		std::string semanticName, 
		uint32_t semanticIndex, 
		DXGI_FORMAT format
	);

	void SetShader(const Shader& shader_);

	void Create(
		const RootSignature& rootSignature,
		Pipeline::Blend blend_,
		Pipeline::CullMode cullMode_,
		Pipeline::SolidState solidState_,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_,
		uint32_t numRenderTarget_ = 1,
		bool isDepth_ = true
	);

	void Use() const;

	bool IsSame(
		const Shader& shader_,
		Pipeline::Blend blend_,
		Pipeline::CullMode cullMode_,
		Pipeline::SolidState solidState_,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_,
		uint32_t numRenderTarget_,
		ID3D12RootSignature* rootSignature_,
		bool isDepth_
	);

/// <summary>
/// メンバ変数
/// </summary>
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	Shader shader;

	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexInput;
	std::vector<std::string> semanticNames;

	Pipeline::Blend blend;
	Pipeline::CullMode cullMode;
	Pipeline::SolidState solidState;
	uint32_t numRenderTarget;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
	bool isDepth;

	ID3D12RootSignature* rootSignature;
};