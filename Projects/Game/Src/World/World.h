#pragma once
#include "Framework/Framework.h"

class World final : public Framework {
public:
	World() = default;
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
};