#pragma once
#include <memory>
#include <thread>
#include <queue>
#include "Mesh/Mesh.h"
#include "Utils/UtilsLib/UtilsLib.h"

/// <summary>
/// メッシュを管理
/// </summary>
class MeshManager final {
private:
	MeshManager() = default;
	MeshManager(const MeshManager&) = delete;
	MeshManager(MeshManager&&) = delete;
	~MeshManager();

	MeshManager& operator=(const MeshManager&) = delete;
	MeshManager& operator=(MeshManager&&) = delete;

public:
	static MeshManager* const GetInstance();

	static void Initialize();
	static void Finalize();

private:
	static MeshManager* instance_;

public:
	Mesh* LoadObj(const std::string& objFileName);
	void LoadObj(const std::string& objFileName, Mesh** mesh_);

	void ResetDrawCount();

	void Draw();


public:
	void ThreadLoad();

	void CheckLoadFinish();

	void JoinThread();

	bool IsNowThreadLoading() const;

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs_;

	std::queue<std::pair<std::string, Mesh**>> threadMeshBuff_;
	std::thread load_;
	bool isThreadFinish_;
	bool isNowThreadLoading_;
};