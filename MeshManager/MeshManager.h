#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Mesh/Mesh.h"


class MeshManager final {
private:
	MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	~MeshManager() = default;

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

public:
	static MeshManager* const GetInstance();

public:
	Mesh* LoadObj(const std::string& objFileName);

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;
};