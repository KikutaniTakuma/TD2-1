#include "MeshManager.h"
#include "Engine/Engine.h"

MeshManager* MeshManager::instance_ = nullptr;

MeshManager* const MeshManager::GetInstance() {
	assert(instance_);
	return instance_;
}

MeshManager::~MeshManager() {
	if (load_.joinable()) {
		load_.join();
	}

	meshs_.clear();

	while (!threadMeshBuff_.empty()) {
		threadMeshBuff_.pop();
	}
}

void MeshManager::Initialize() {
	instance_ = new MeshManager{};
	assert(instance_);
}
void MeshManager::Finalize() {
	delete instance_;
	instance_ = nullptr;
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

void MeshManager::LoadObj(const std::string& objFileName, Mesh** mesh) {
	threadMeshBuff_.push(std::make_pair(objFileName, mesh));
}

void MeshManager::ResetDrawCount() {
	for (auto& i : meshs_) {
		i.second->ResetDrawCount();
	}
}

void MeshManager::Draw() {
	for (auto& i : meshs_) {
		i.second->Draw();
	}
}

void MeshManager::ThreadLoad() {
	// 読み込み予定の物があるかつ今読み込み中ではない
	if (!threadMeshBuff_.empty() && !load_.joinable()) {
		auto loadProc = [this]() {
			isNowThreadLoading_ = true;
			while (!threadMeshBuff_.empty()) {
				if (Engine::IsFinalize()) {
					break;
				}
				// ロードするmeshを取得
				auto& front = threadMeshBuff_.front();
				
				// ロード済みか判定
				auto mesh = meshs_.find(front.first);

				// ロード済みではなかったらロードして代入
				if (mesh == meshs_.end()) {
					meshs_[front.first] = std::make_unique<Mesh>();
					meshs_[front.first]->ThreadLoadObj(front.first);
					if (Engine::IsFinalize()) {
						break;
					}
					(*front.second) = meshs_[front.first].get();
				}
				// ロード済みだったらロード済みのmeshポインタを代入
				else {
					if (Engine::IsFinalize()) {
						break;
					}
					(*front.second) = mesh->second.get();
				}
				threadMeshBuff_.pop();
			}

			// テクスチャの読み込みが終わるまでループ
			bool isTextureLoadFinish = false;
			while (!isTextureLoadFinish) {
				if (Engine::IsFinalize()) {
					break;
				}

				for (auto& i : meshs_) {
					i.second->CheckModelTextureLoadFinish();
				}
				for (auto& i : meshs_) {
					if (!i.second->isLoad_) {
						isTextureLoadFinish = false;
						break;
					}
					else {
						isTextureLoadFinish = true;
					}
				}
			}

			isThreadFinish_ = true;
			isNowThreadLoading_ = false;
			};

		// 非同期開始
		load_ = std::thread(loadProc);
	}
}

void MeshManager::CheckLoadFinish() {
	if (isThreadFinish_) {
		for (auto& i : meshs_) {
			if (!i.second->isLoad_) {
				return;
			}
		}

		isThreadFinish_ = false;
	}
}
void MeshManager::JoinThread() {
	if (isThreadFinish_ && load_.joinable()) {
		load_.join();
	}
}

bool MeshManager::IsNowThreadLoading() const {
	return isNowThreadLoading_;
}