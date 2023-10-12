#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

#include "Utils/UtilsLib/UtilsLib.h"

#include <array>

class Particle {
public:
	struct MatrixData {
		Mat4x4 wvpMat;
	};

	struct VertexData {
		Vector3 position;
		Vector2 uv;
	};

private:
	struct WorldTransForm {
		Vector2 scale;
		Vector3 rotate;
		Vector3 pos;
		uint32_t color;
		
		Vector3 movePos;
		bool isActive;

		// スタートした時間
		std::chrono::steady_clock::time_point startTime;

		// 消える時間
		std::chrono::milliseconds deathTime;
	};

	enum class EmitterType {
		Circle,
		Cube,
	};

	struct Emitter {
		// エミッターの場所
		Vector3 pos;

		// エミッターの大きさ
		Vector3 size;

		// エミッターのタイプ(立方体か球か)
		EmitterType type;

		// 球のときの半径
		float circleSize;

		// 球のときのランダム範囲
		std::pair<Vector3, Vector3> rotate;

		uint32_t particleMaxNum;
	};

	struct Setting {
		Emitter emitter;

		///
		/// 乱数の範囲
		/// 

		// 大きさ
		std::pair<Vector2, Vector2> size;

		// 移動(速度)
		std::pair<Vector3, Vector3> velocity;

		// 移動方向
		std::pair<Vector3, Vector3> rotate;


		// 一度にいくつ出すか(数)
		std::pair<uint32_t, uint32_t> particleNum;

		// 出す頻度(milliseconds)
		std::pair<uint32_t, uint32_t> freq;

		// 消える時間(milliseconds)
		std::pair<uint32_t, uint32_t> death;

		// 色
		std::pair<uint32_t, uint32_t> color;

		///
		/// 
		/// 
		
		// スタートした時間
		std::chrono::steady_clock::time_point startTime;

		// 前に出した時間
		std::chrono::steady_clock::time_point durationTime;

		// 有効時間
		std::chrono::milliseconds validTime;

		// 今有効か
		UtilsLib::Flg isValid;
	};

public:
	Particle() = delete;
	Particle(uint32_t indexNum);
	Particle(const Particle&);
	Particle(Particle&&) noexcept;
	~Particle();

public:
	Particle& operator=(const Particle& right);
	Particle& operator=(Particle&& right) noexcept;

	inline WorldTransForm& operator[](size_t index) {
		return wtfs[index];
	}

	inline const WorldTransForm& operator[](size_t index) const {
		return wtfs[index];
	}


	/// <summary>
	/// 静的メンバ関数
	/// </summary>
public:
	static void Initialize(
		const std::string& vsFileName = "./Resources/Shaders/ParticleShader/Particle.VS.hlsl",
		const std::string& psFileName = "./Resources/Shaders/ParticleShader/Particle.PS.hlsl"
	);

	static void Finalize();

private:
	static void LoadShader(const std::string& vsFileName, const std::string& psFileName);

	static void CreateGraphicsPipeline();

	/// <summary>
	/// 静的メンバ変数
	/// </summary>
private:
	static std::array<Pipeline*, size_t(Pipeline::Blend::BlendTypeNum)> graphicsPipelineState;
	static Shader shader;

	static D3D12_INDEX_BUFFER_VIEW indexView;
	static Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;


public:
	void LoadTexture(const std::string& fileName);
	void ThreadLoadTexture(const std::string& fileName);

public:
	void Update();

	void Draw(
		const Mat4x4& viewProjection,
		Pipeline::Blend blend = Pipeline::Blend::None
	);

	void Debug(const std::string& guiName);

	Vector2 GetTexSize() const {
		if (tex) {
			return tex->getSize();
		}
		else {
			return Vector2::zero;
		}
	}

	/// <summary>
	/// アニメーションスタート関数
	/// </summary>
	/// <param name="aniUvPibot">アニメーションをスタートさせる場所</param>
	void AnimationStart(float aniUvPibot = 0.0f);

	/// <summary>
	/// アニメーション関数
	/// </summary>
	/// <param name="aniSpd">アニメーション速度(milliseconds)</param>
	/// <param name="isLoop">アニメーションをループさせるか</param>
	/// <param name="aniUvStart">アニメーションをスタートさせる場所</param>
	/// <param name="aniUvEnd">アニメーションを終わらせる場所</param>
	void Animation(size_t aniSpd, bool isLoop, float aniUvStart, float aniUvEnd);

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void AnimationPause();

	/// <summary>
	/// アニメーションを再スタート
	/// </summary>
	void AnimationRestart();

	/// <summary>
	/// アニメーションしているかを取得
	/// </summary>
	/// <returns>アニメーションフラグ</returns>
	bool GetIsAnimation()const {
		return isAnimation_;
	}

	void Resize(uint32_t index) {
		wvpMat.Resize(index);
		srvHeap.CreateStructuredBufferView(wvpMat, 1);
		colorBuf.Resize(index);
		srvHeap.CreateStructuredBufferView(colorBuf, 2);
		wtfs.resize(index);
	}

public:
	Vector2 uvPibot;
	Vector2 uvSize;

	std::deque<Setting> settings;

private:
	// ループするか
	UtilsLib::Flg isLoop_;

	uint32_t currentSettingIndex;
	uint32_t currentParticleIndex;

	std::vector<WorldTransForm> wtfs;

	ShaderResourceHeap srvHeap;


	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	StructuredBuffer<Mat4x4> wvpMat;
	StructuredBuffer<Vector4> colorBuf;

	Texture* tex;
	bool isLoad;

	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;
public:
	float uvPibotSpd_;
};