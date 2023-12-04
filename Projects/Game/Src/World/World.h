#pragma once
#include "Framework/Framework.h"
#include <memory>
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

class World final : public Framework {
public:
	World();
	World(const World&) = delete;
	World(World&&) = delete;
	~World() = default;

	World& operator=(const World&) = delete;
	World& operator=(World&&) = delete;

private:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw()override;

private:
	class SceneManager* sceneManager_;

	class ParticleEditor* particleEditor_;

	std::unique_ptr<Texture2D> inactiveSprite_;
	std::unique_ptr<Mat4x4> staticCameraMatrix_;
};