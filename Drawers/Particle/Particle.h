#pragma once
#include "TextureManager/TextureManager.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "Engine/ShaderResource/ShaderResourceHeap.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

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
		
		Vector3 movePos;
		bool isActive;
	};

	enum class EmitterType {
		Circle,
		Cube,
	};

	struct Emitter {
		Vector3 pos;

		Vector3 size;
		EmitterType type;
		float circleSize;
	};

	struct Setting {
		Emitter emitter;
		
		// 大きさ
		Vector3 startSize;
		Vector3 endSize;

		// 移動
		Vector3 startVelocity;
		Vector3 endVelocity;

		// 一度にいくつ出すか
		uint32_t startParticleNum;
		uint32_t endParticleNum;

		// 出す頻度
		std::chrono::milliseconds startFreq;
		std::chrono::milliseconds endFreq;

		// 消える時間
		std::chrono::milliseconds startDeath;
		std::chrono::milliseconds endDeath;
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
		wtfs.resize(index);
		for (size_t i = 0; i < wtfs.size(); i++) {
			wtfs[i].scale = Vector2::identity * 512.0f;
			wtfs[i].pos.x = 10.0f * i;
			wtfs[i].pos.y = 10.0f * i;
			wtfs[i].pos.z += 0.3f;
		}
	}

public:
	Vector2 uvPibot;
	Vector2 uvSize;

	uint32_t color;

	std::deque<Setting> ssettings;

private:
	std::vector<WorldTransForm> wtfs;

	ShaderResourceHeap srvHeap;


	D3D12_VERTEX_BUFFER_VIEW vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	StructuredBuffer<Mat4x4> wvpMat;
	ConstBuffer<Vector4> colorBuf;

	Texture* tex;
	bool isLoad;

	// アニメーション変数
	std::chrono::steady_clock::time_point aniStartTime_;
	float aniCount_;
	bool isAnimation_;
public:
	float uvPibotSpd_;
};