#include "Mesh.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/Engine.h"
#include "TextureManager/TextureManager.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <deque>
#include <algorithm>
#include <filesystem>

Mesh::Mesh() :
	meshs_(),
	isLoad_(false)
{}

Mesh::~Mesh() {

}

void Mesh::LoadObj(const std::string& objFileName) {
	if (!isLoad_) {
		std::ifstream objFile(objFileName);
		assert(objFile);
		if (objFile.fail()) {
			if (std::filesystem::exists(std::filesystem::path(objFileName))) {
				ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : " + objFileName + " open failed", "Model");
			}
			else {
				ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : Not found objFile : " + objFileName, "Model");
			}

			return;
		}

		std::vector<Vector4> posDatas(0);

		std::vector<Vector3> normalDatas(0);

		std::vector<Vector2> uvDatas(0);

		std::unordered_map<std::string, std::vector<IndexData>> indexDatas(0);
		std::unordered_map<std::string, std::vector<IndexData>>::iterator indicesItr;

		std::string lineBuf;

		while (std::getline(objFile, lineBuf)) {
			std::string identifier;
			std::istringstream line(lineBuf);
			line >> identifier;
			if (identifier == "v") {
				Vector4 buf;
				line >> buf.vec.x >> buf.vec.y >> buf.vec.z;
				buf.vec.x *= -1.0f;
				buf.vec.w = 1.0f;

				posDatas.push_back(buf);
			}
			else if (identifier == "vn") {
				Vector3 buf;
				line >> buf.x >> buf.y >> buf.z;
				buf.x *= -1.0f;
				normalDatas.push_back(buf);
			}
			else if (identifier == "vt") {
				Vector2 buf;
				line >> buf.x >> buf.y;
				buf.y = 1.0f - buf.y;
				uvDatas.push_back(buf);
			}
			else if (identifier == "f") {
				std::string buf;
				std::vector<float> posBuf(0);
				std::array<IndexData, 3> indcoes;
				auto idnexItr = indcoes.rbegin();
				while (std::getline(line, buf, ' '))
				{
					/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
					std::string num[3];
					int32_t count = 0;
					if (std::any_of(buf.cbegin(), buf.cend(), isdigit)) {
						for (auto& ch : buf) {
							if (ch == '/') {
								count++;
							}
							else { num[count] += ch; }
						}
					}

					// エラーチェック
					if (idnexItr == indcoes.rend()) {
						//assert(!"Obj Load Error : Cannot Load Rectangles or more");
						ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : Not supported for rectangles or more", "Model");
						objFile.close();
						return;
					}

					if (count == 2) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->uvNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[2]) - 1);
						idnexItr++;
					}
					else if (count == 1) {
						idnexItr->vertNum = static_cast<uint32_t>(std::stoi(num[0]) - 1);
						idnexItr->normalNum = static_cast<uint32_t>(std::stoi(num[1]) - 1);
						idnexItr++;
					}
				}
				for (auto& i : indcoes) {
					indicesItr->second.push_back(i);
				}
			}
			else if (identifier == "usemtl") {
				std::string useMtlName;
				line >> useMtlName;
				indexDatas.insert({ useMtlName,std::vector<IndexData>(0) });
				indicesItr = indexDatas.find(useMtlName);
				meshs_[useMtlName];
			}
			else if (identifier == "mtllib") {
				std::string mtlFileName;
				std::filesystem::path path = objFileName;

				line >> mtlFileName;

				LoadMtl(path.parent_path().string() + "/" + mtlFileName);
			}
		}
		objFile.close();

		for (auto i : indexDatas) {
			meshs_[i.first].vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertData) * indexDatas[i.first].size());
			assert(meshData[i.first].vertexBuffer);


			// リソースの先頭のアドレスから使う
			meshs_[i.first].vertexView.BufferLocation = meshs_[i.first].vertexBuffer->GetGPUVirtualAddress();
			// 使用するリソースのサイズは頂点3つ分のサイズ
			meshs_[i.first].vertexView.SizeInBytes = static_cast<UINT>(sizeof(VertData) * indexDatas[i.first].size());
			// 1頂点当たりのサイズ
			meshs_[i.first].vertexView.StrideInBytes = sizeof(VertData);

			// 頂点リソースにデータを書き込む
			meshs_[i.first].vertexMap = nullptr;
			// 書き込むためのアドレスを取得
			meshs_[i.first].vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshs_[i.first].vertexMap));

			for (int32_t j = 0; j < indexDatas[i.first].size(); j++) {
				meshs_[i.first].vertexMap[j].position = posDatas[indexDatas[i.first][j].vertNum];
				meshs_[i.first].vertexMap[j].normal = normalDatas[indexDatas[i.first][j].normalNum];
				if (!uvDatas.empty()) {
					meshs_[i.first].vertexMap[j].uv = uvDatas[indexDatas[i.first][j].uvNum];
				}
			}


			meshs_[i.first].vertNum = static_cast<uint32_t>(indexDatas[i.first].size());
		}
		isLoad_ = true;
	}
}

void Mesh::LoadMtl(const std::string& fileName) {
	std::ifstream file(fileName);
	assert(file);
	if (!file) { ErrorCheck::GetInstance()->ErrorTextBox("LoadMtl() : Not Found mtlFile", "Model"); }

	std::string lineBuf;
	std::unordered_map<std::string, Texture*>::iterator texItr;
	std::unordered_map<std::string, ShaderResourceHeap>::iterator hepaItr;

	std::string useMtlName;
	while (std::getline(file, lineBuf)) {
		std::string identifier;
		std::istringstream line(lineBuf);

		line >> identifier;
		if (identifier == "map_Kd") {
			std::string texName;
			std::filesystem::path path = fileName;

			line >> texName;

			texItr->second = TextureManager::GetInstance()->LoadTexture(path.parent_path().string() + "/" + texName);
			hepaItr->second.CreateTxtureView(texItr->second);
		}
		else if (identifier == "newmtl") {
			line >> useMtlName;
			texs_.insert({ useMtlName, nullptr });
			texItr = texs_.find(useMtlName);
			SRVHeap_.insert({ useMtlName , ShaderResourceHeap() });
			hepaItr = SRVHeap_.find(useMtlName);
			hepaItr->second.InitializeReset(16);
		}
	}

	for (auto& i : texs_) {
		if (i.second == nullptr || !(*i.second)) {
			i.second = TextureManager::GetInstance()->GetWhiteTex();
			SRVHeap_[i.first].CreateTxtureView(i.second);
		}
	}
}

void Mesh::Use() {

}