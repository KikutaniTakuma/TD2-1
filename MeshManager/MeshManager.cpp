#include "MeshManager.h"

MeshManager* const MeshManager::GetInstance() {
	static MeshManager instance;

	return &instance;
}

Mesh* MeshManager::LoadObj(const std::string& objFileName) {
	auto mesh = meshs_.find(objFileName);

	if (mesh == meshs_.end()) {
		meshs_[objFileName];
		meshs_[objFileName] = std::make_unique<Mesh>();
		meshs_[objFileName]->LoadObj(objFileName);
	}

	return meshs_[objFileName].get();
}