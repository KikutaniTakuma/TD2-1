#pragma once
#include "Pipeline/Pipeline.h"
#include <list>
#include <vector>
#include <memory>
#include <tuple>
class RootSignature;

class PipelineManager {
/// <summary>
/// コンストラクタ
/// </summary>
private:
	PipelineManager();
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager(PipelineManager&&) = delete;
	~PipelineManager() = default;

	PipelineManager& operator=(const PipelineManager&) = delete;
	PipelineManager& operator=(PipelineManager&&) = delete;

/// <summary>
/// 静的関数
/// </summary>
public:
	static void Initialize();
	static void Finalize();

	///
	/// 下の4つの関数を使った後にCreateする
	/// 
	
	/// <summary>
	/// ルートシグネチャを生成(この関数を呼び出したらそのルートシグネチャがPSOに使われる)
	/// </summary>
	/// <param name="rootParamater_">ルートパラメータ</param>
	/// <param name="isTexture_">テクスチャを使う場合はtrue</param>
	static void CreateRootSgnature(D3D12_ROOT_PARAMETER* rootParamater_, size_t rootParamaterSize_, bool isTexture_);

	static void SetRootSgnature(RootSignature* rootSignature);

	/// <summary>
	/// 頂点シェーダの入力設定(呼び出し毎に複数設定可能)
	/// </summary>
	/// <param name="semanticName_">セマンティクス名</param>
	/// <param name="semanticIndex_">セマンティクスインデックス</param>
	/// <param name="format_">フォーマット</param>
	static void SetVertexInput(std::string semanticName_, uint32_t semanticIndex_, DXGI_FORMAT format_);

	/// <summary>
	/// 使うシェーダの設定
	/// </summary>
	/// <param name="shader_">シェーダ</param>
	static void SetShader(const Shader& shader_);

	/// <summary>
	/// ステイタスの設定
	/// </summary>
	/// <param name="blend_">ブレンドの設定</param>
	/// <param name="solidState_">ソリッド設定</param>
	/// <param name="cullMode_">カリング設定</param>
	/// <param name="numRenderTarget_">レンダーターゲットの数</param>
	static void SetState(
		Pipeline::Blend blend_,
		Pipeline::SolidState solidState_,
		Pipeline::CullMode cullMode_ = Pipeline::CullMode::Back,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		uint32_t numRenderTarget_ = 1u
	);

	static void IsDepth(bool isDepth_ = true);

	/// <summary>
	/// 設定したものでPSOの生成
	/// </summary>
	/// <returns>psoのポインタ(勝手にdeleteしてはいけない)</returns>
	static Pipeline* Create();

	/// <summary>
	/// CreateRootSgnature(), SetVertexInput(), SetShader(), SetState()で設定した値をリセット
	/// </summary>
	static void StateReset();

/// <summary>
/// シングルトンインスタンス
/// </summary>
private:
	static PipelineManager* instance;

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::list<std::unique_ptr<Pipeline>> pipelines;
	std::list<std::unique_ptr<RootSignature>> rootSignatures;

	RootSignature* rootSignature;

	Shader shader;
	Pipeline::Blend blend;
	Pipeline::CullMode cullMode;
	Pipeline::SolidState solidState;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType;
	uint32_t numRenderTarget;
	bool isDepth;

	std::vector<std::tuple<std::string, uint32_t, DXGI_FORMAT>> vertexInputStates;
};