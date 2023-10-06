#pragma once
#include <unordered_map>
#include <string>

class MeshManager final {
private:
	MeshManager();
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	~MeshManager();

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

public:
	void LoadObj(const std::string& objFileName);

private:
	std::unordered_map<std::string, >
};