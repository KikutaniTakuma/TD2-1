#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
#include <filesystem>
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/PipelineManager/PipelineManager.h"


Model::Model() :
	pos(),
	rotate(),
	scale(Vector3::identity),
	color(0xffffffff),
	parent(nullptr),
	meshData(),
	shader(),
	pipeline(nullptr),
	loadObjFlg(false),
	loadShaderFlg(false),
	createGPFlg(false),
	wvpData(),
	dirLig(),
	colorBuf(),
	SRVHeap(),
	tex(0),
	drawIndexNumber(0),
	maxDrawIndex(1)
{
	wvpData.resize(maxDrawIndex);
	for (auto& i : wvpData) {
		i.shaderRegister = 0;
		i->worldMat = MakeMatrixIndentity();
		i->viewProjectoionMat = MakeMatrixIndentity();
	}

	dirLig.resize(maxDrawIndex);
	for (auto& i : dirLig) {
		i.shaderRegister = 1;
		i->ligDirection = { 1.0f,-1.0f,-1.0f };
		i->ligDirection = dirLig.back()->ligDirection.Normalize();
		Vector4 colorTmp = UintToVector4(0xffffadff);
		i->ligColor = colorTmp.GetVector3();

		i->ptPos = { 5.0f,5.0f,5.0f };
		i->ptColor = { 15.0f,15.0f,15.0f };
		i->ptRange = 10.0f;
	}

	colorBuf.resize(maxDrawIndex);
	for (auto& i : colorBuf) {
		i.shaderRegister = 2;
		*i = UintToVector4(color);
	}
}

Model::Model(UINT maxDrawIndex_) :
	pos(),
	rotate(),
	scale(Vector3::identity),
	color(0xffffffff),
	parent(nullptr),
	meshData(),
	shader(),
	pipeline(nullptr),
	loadObjFlg(false),
	loadShaderFlg(false),
	createGPFlg(false),
	wvpData(),
	dirLig(),
	colorBuf(),
	SRVHeap(),
	tex(0),
	drawIndexNumber(0),
	maxDrawIndex(maxDrawIndex_)
{
	if (maxDrawIndex < 1) {
		maxDrawIndex = 1;
	}

	wvpData.resize(maxDrawIndex);
	for (auto& i : wvpData) {
		i.shaderRegister = 0;
		i->worldMat = MakeMatrixIndentity();
		i->viewProjectoionMat = MakeMatrixIndentity();
	}

	dirLig.resize(maxDrawIndex);
	for (auto& i : dirLig) {
		i.shaderRegister = 1;
		i->ligDirection = { 1.0f,-1.0f,-1.0f };
		i->ligDirection = dirLig.back()->ligDirection.Normalize();
		Vector4 colorTmp = UintToVector4(0xffffadff);
		i->ligColor = colorTmp.GetVector3();

		i->ptPos = { 5.0f,5.0f,5.0f };
		i->ptColor = { 15.0f,15.0f,15.0f };
		i->ptRange = 10.0f;
	}

	colorBuf.resize(maxDrawIndex);
	for (auto& i : colorBuf) {
		i.shaderRegister = 2;
		*i = UintToVector4(color);
	}
}

void Model::LoadObj(const std::string& fileName) {
	if (!loadObjFlg) {
		std::ifstream objFile(fileName);
		assert(objFile);
		if (!objFile) {
			ErrorCheck::GetInstance()->ErrorTextBox("LoadObj() : Not found objFile : " + fileName, "Model");
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
				meshData[useMtlName];
			}
			else if (identifier == "mtllib") {
				std::string mtlFileName;
				std::filesystem::path path = fileName;

				line >> mtlFileName;

				LoadMtl(path.parent_path().string() + "/" + mtlFileName);
			}
		}
		objFile.close();

		for (auto i : indexDatas) {
			meshData[i.first].vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertData) * indexDatas[i.first].size());
			assert(meshData[i.first].vertexBuffer);


			// リソースの先頭のアドレスから使う
			meshData[i.first].vertexView.BufferLocation = meshData[i.first].vertexBuffer->GetGPUVirtualAddress();
			// 使用するリソースのサイズは頂点3つ分のサイズ
			meshData[i.first].vertexView.SizeInBytes = static_cast<UINT>(sizeof(VertData) * indexDatas[i.first].size());
			// 1頂点当たりのサイズ
			meshData[i.first].vertexView.StrideInBytes = sizeof(VertData);

			// 頂点リソースにデータを書き込む
			meshData[i.first].vertexMap = nullptr;
			// 書き込むためのアドレスを取得
			meshData[i.first].vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData[i.first].vertexMap));

			for (int32_t j = 0; j < indexDatas[i.first].size(); j++) {
				meshData[i.first].vertexMap[j].position = posDatas[indexDatas[i.first][j].vertNum];
				meshData[i.first].vertexMap[j].normal = normalDatas[indexDatas[i.first][j].normalNum];
				if (!uvDatas.empty()) {
					meshData[i.first].vertexMap[j].uv = uvDatas[indexDatas[i.first][j].uvNum];
				}
			}


			meshData[i.first].vertNum = static_cast<uint32_t>(indexDatas[i.first].size());
		}
		loadObjFlg = true;
	}
}

void Model::LoadMtl(const std::string fileName) {
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
			tex.insert({ useMtlName, nullptr });
			texItr = tex.find(useMtlName);
			SRVHeap.insert({ useMtlName , ShaderResourceHeap() });
			hepaItr = SRVHeap.find(useMtlName);
			hepaItr->second.InitializeReset(16);
		}
	}

	for (auto& i : tex) {
		if (i.second == nullptr || !(*i.second)) {
			i.second = TextureManager::GetInstance()->GetWhiteTex();
			SRVHeap[i.first].CreateTxtureView(i.second);
		}
	}
}

void Model::LoadShader(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	if (!loadShaderFlg) {
		shader.vertex = ShaderManager::LoadVertexShader(vertex);
		shader.pixel = ShaderManager::LoadPixelShader(pixel);
		if (geometory.size() != 0LLU) {
			shader.geometory = ShaderManager::LoadGeometoryShader(geometory);
		}
		if (hull.size() != 0LLU && geometory.size() != 0LLU) {
			shader.hull = ShaderManager::LoadHullShader(hull);
			shader.domain = ShaderManager::LoadHullShader(domain);
		}
		loadShaderFlg = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg && loadObjFlg) {
		std::array<D3D12_ROOT_PARAMETER, 4> paramates;
		paramates[0] = SRVHeap.begin()->second.GetParameter();
		paramates[1] = wvpData.front().GetRoootParamater();
		paramates[2] = dirLig.front().GetRoootParamater();
		paramates[3] = colorBuf.front().GetRoootParamater();
		PipelineManager::CreateRootSgnature(paramates.data(), paramates.size(), true);

		PipelineManager::SetShader(shader);

		PipelineManager::SetVertexInput("POSITION", 0u, DXGI_FORMAT_R32G32B32A32_FLOAT);
		PipelineManager::SetVertexInput("NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT);
		PipelineManager::SetVertexInput("TEXCOORD", 0u, DXGI_FORMAT_R32G32_FLOAT);

		PipelineManager::SetState(Pipeline::Blend::None, Pipeline::SolidState::Solid);

		pipeline = PipelineManager::Create();

		PipelineManager::StateReset();

		createGPFlg = true;
	}
}

void Model::Update() {
	drawIndexNumber = 0;
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg);
	if (drawIndexNumber >= maxDrawIndex) {
		drawIndexNumber = 0;
	}

	wvpData[drawIndexNumber]->worldMat.HoriAffin(scale, rotate, pos);
	if (parent) {
		wvpData[drawIndexNumber]->worldMat *= MakeMatrixTransepose(parent->wvpData.back()->worldMat);
	}
	wvpData[drawIndexNumber]->worldMat.Transepose();
	wvpData[drawIndexNumber]->viewProjectoionMat = viewProjectionMat;

	*colorBuf[drawIndexNumber] = UintToVector4(color);

	dirLig[drawIndexNumber]->eyePos = cameraPos;


	auto commandlist = Engine::GetCommandList();

	if (!pipeline) {
		ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Model");
		return;
	}

	[[maybe_unused]]size_t indexVertex = 0;

	for (auto& i : meshData) {
		pipeline->Use();
		SRVHeap[i.first].Use();

		commandlist->IASetVertexBuffers(0, 1, &i.second.vertexView);

		commandlist->SetGraphicsRootConstantBufferView(1, wvpData[drawIndexNumber].GetGPUVtlAdrs());
		commandlist->SetGraphicsRootConstantBufferView(2, dirLig[drawIndexNumber].GetGPUVtlAdrs());
		commandlist->SetGraphicsRootConstantBufferView(3, colorBuf[drawIndexNumber].GetGPUVtlAdrs());

		commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
	}

	drawIndexNumber++;
}

void Model::Debug(const std::string& guiName) {
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf.back()->color.r);
	ImGui::DragFloat3("ligDirection", &dirLig.back()->ligDirection.x, 0.01f);
	dirLig.back()->ligDirection = dirLig.back()->ligDirection.Normalize();
	ImGui::DragFloat3("ligColor", &dirLig.back()->ligColor.x, 0.01f);
	ImGui::DragFloat3("ptPos", &dirLig.back()->ptPos.x, 0.01f);
	ImGui::DragFloat3("ptColor", &dirLig.back()->ptColor.x, 0.01f);
	ImGui::DragFloat("ptRange", &dirLig.back()->ptRange);
	ImGui::End();
}

Model::~Model() {
	for (auto& i : meshData) {
		if (i.second.vertexBuffer) {
			i.second.vertexBuffer->Release();
		}
	}
}