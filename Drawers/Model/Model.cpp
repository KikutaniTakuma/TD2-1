#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
#include <climits>
#include <filesystem>
#include "Utils/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/PipelineManager/PipelineManager.h"
#include "MeshManager/MeshManager.h"

Shader Model::shader = {};

Pipeline* Model::pipeline = {};
bool Model::loadShaderFlg = false;
bool Model::createGPFlg = false;

void Model::Initialize(
	const std::string& vertex,
	const std::string& pixel,
	const std::string& geometory,
	const std::string& hull,
	const std::string& domain
) {
	LoadShader(vertex, pixel, geometory, hull, domain);
	CreateGraphicsPipeline();
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
	if (loadShaderFlg) {
		std::array<D3D12_DESCRIPTOR_RANGE,1> range={};
		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

		std::array<D3D12_ROOT_PARAMETER, 4> paramates = {};
		paramates[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		paramates[0].DescriptorTable.pDescriptorRanges = range.data();
		paramates[0].DescriptorTable.NumDescriptorRanges = UINT(range.size());

		paramates[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[1].Descriptor.ShaderRegister = 0;

		paramates[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[2].Descriptor.ShaderRegister = 1;

		paramates[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		paramates[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		paramates[3].Descriptor.ShaderRegister = 2;

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

Model::Model() :
	pos(),
	rotate(),
	scale(Vector3::identity),
	color(std::numeric_limits<uint32_t>::max()),
	parent(nullptr),
	mesh(nullptr),
	data(),
	loadObjFlg(false),
	wvpData(),
	dirLig(),
	colorBuf()
{

	wvpData.shaderRegister = 0;
	wvpData->worldMat = MakeMatrixIndentity();
	wvpData->viewProjectoionMat = MakeMatrixIndentity();


	dirLig.shaderRegister = 1;
	light.ligDirection = { 1.0f,-1.0f,-1.0f };
	light.ligDirection = dirLig->ligDirection.Normalize();
	light.ligColor = UintToVector4(0xffffadff).GetVector3();

	light.ptPos = Vector3::zero;
	light.ptColor = Vector3::zero;
	light.ptRange = 0.0f;

	colorBuf.shaderRegister = 2;
	*colorBuf = UintToVector4(color);
}

Model::Model(const Model& right) :
	Model()
{
	*this = right;
}
Model::Model(Model&& right) noexcept:
	Model()
{
	*this = std::move(right);
}

Model& Model::operator=(const Model& right) {
	pos = right.pos;
	rotate = right.rotate;
	scale = right.scale;
	color = right.color;
	parent = right.parent;

	// 自身がロード済みだったらResourceを解放する
	if (loadObjFlg) {
		for (auto& i : data) {
			if (i.second.resource.first) {
				i.second.resource.first->Release();
			}
		}

		data.clear();
	}

	// rightがロード済みだったら
	if (right.loadObjFlg) {
		mesh = right.mesh;

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("operator=() : right mesh is nullptr", "Model");
			return *this;
		}

		data = mesh->CreateResource();

		loadObjFlg = true;
	}

	// 定数バッファの値をコピー
	*wvpData = *right.wvpData;
	*dirLig = *right.dirLig;
	*colorBuf = *right.colorBuf;

	return *this;
}

Model& Model::operator=(Model&& right) noexcept {
	pos = std::move(right.pos);
	rotate = std::move(right.rotate);
	scale = std::move(right.scale);
	color = std::move(right.color);
	parent = std::move(right.parent);

	// 自身がロード済みだったらResourceを解放する
	if (loadObjFlg) {
		for (auto& i : data) {
			if (i.second.resource.first) {
				i.second.resource.first->Release();
			}
		}

		data.clear();
	}

	// rightがロード済みだったら
	if (right.loadObjFlg) {
		mesh = right.mesh;

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("operator=() : right mesh is nullptr", "Model");
			return *this;
		}

		data = mesh->CreateResource();

		loadObjFlg = true;
	}

	// 定数バッファの値をコピー
	*wvpData = *right.wvpData;
	*dirLig = *right.dirLig;
	*colorBuf = *right.colorBuf;

	return *this;
}

void Model::LoadObj(const std::string& fileName) {
	if (!loadObjFlg) {
		mesh = MeshManager::GetInstance()->LoadObj(fileName);

		if (!mesh) {
			ErrorCheck::GetInstance()->ErrorTextBox("LoadObj : mesh is nullptr", "Model");
			return;
		}

		data = mesh->CreateResource();

		loadObjFlg = true;
	}
}

void Model::Update() {
	*dirLig = light;
}

void Model::Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos) {
	assert(createGPFlg);
	if (loadObjFlg) {
		wvpData->worldMat.HoriAffin(scale, rotate, pos);
		if (parent) {
			wvpData->worldMat *= MakeMatrixTransepose(parent->wvpData->worldMat);
		}
		wvpData->worldMat.Transepose();
		wvpData->viewProjectoionMat = viewProjectionMat;

		*colorBuf = UintToVector4(color);

		dirLig->eyePos = cameraPos;


		auto commandlist = Engine::GetCommandList();

		if (!pipeline) {
			ErrorCheck::GetInstance()->ErrorTextBox("pipeline is nullptr", "Model");
			return;
		}

		[[maybe_unused]] size_t indexVertex = 0;

		for (auto& i : data) {
			pipeline->Use();
			i.second.tex->Use(0);


			commandlist->SetGraphicsRootConstantBufferView(1, wvpData.GetGPUVtlAdrs());
			commandlist->SetGraphicsRootConstantBufferView(2, dirLig.GetGPUVtlAdrs());
			commandlist->SetGraphicsRootConstantBufferView(3, colorBuf.GetGPUVtlAdrs());

			commandlist->IASetVertexBuffers(0, 1, &i.second.resource.second);
			commandlist->DrawInstanced(i.second.vertNum, 1, 0, 0);
		}
	}
}

void Model::Debug(const std::string& guiName) {
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat3("pos", &pos.x, 0.01f);
	ImGui::DragFloat3("rotate", &rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &colorBuf->color.r);
	ImGui::DragFloat3("ligDirection", &dirLig->ligDirection.x, 0.01f);
	dirLig->ligDirection = dirLig->ligDirection.Normalize();
	ImGui::DragFloat3("ligColor", &dirLig->ligColor.x, 0.01f);
	ImGui::DragFloat3("ptPos", &dirLig->ptPos.x, 0.01f);
	ImGui::DragFloat3("ptColor", &dirLig->ptColor.x, 0.01f);
	ImGui::DragFloat("ptRange", &dirLig->ptRange);
	ImGui::End();
}

Model::~Model() {
	for (auto& i : data) {
		if (i.second.resource.first) {
			i.second.resource.first->Release();
		}
	}
}