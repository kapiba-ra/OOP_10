#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "VertexArray.h"
#include "Renderer.h"
#include "Math.h"

Mesh::Mesh()
	: mVertexArray(nullptr)
	, mRadius(0.0f)
	, mSpecPower(100.0f)
	, mBox(Vector3::Infinity, Vector3::NegInfinity)
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string& fileName, Renderer* renderer)
{
	/* gpmesh�t�@�C���������ǂݍ���ł��� */

	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("�t�@�C����������܂��� : %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	// RapidJSON�ŉ�͂���ׁA�p�ӂ��ꂽstream���o�R����
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Mesh %s �͖�����json�ł�", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	if (ver != 1)
	{
		SDL_Log("Mesh %s �̃o�[�W������1�ł͂���܂���", fileName.c_str());
		return false;
	}

	mShaderName = doc["shader"].GetString();

	size_t vertSize = 8; // ���_1�Ɋ܂܂����̐�

	const rapidjson::Value& textures = doc["textures"];
	if (!textures.IsArray() || textures.Size() < 1)
	{
		SDL_Log("Mesh %s ���e�N�X�`���������Ă��܂���", fileName.c_str());
		return false;
	}

	mSpecPower = static_cast<float>(doc["specularPower"].GetFloat());

	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// �e�N�X�`���͊���Load����Ă��邩
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);
		if (t == nullptr)
		{
			// �ă��[�h�����݂�
			t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// �w��̃e�N�X�`�����Ȃ��Ă������悤��
				t = renderer->GetTexture("Assets/Default.png");
			}
		}
		mTextures.emplace_back(t);
	}

	// Load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		SDL_Log("Mesh % s �����_�������Ă��܂���", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);	// �ÓI�Ɋm�ۂ��Ă���	
	mRadius = 0.0f;
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// �v�f�� : 8
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray() || vert.Size() != 8)
		{
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}
		// �e���_��8�̗v�f������,�ŏ���3�v�f��x,y,z���W��\���Ă���
		Vector3 pos(vert[0].GetFloat(), vert[1].GetFloat(), vert[2].GetFloat());
		mRadius = Math::Max(mRadius, pos.LengthSq());
		mBox.UpdateMinMax(pos);	// mBox.mMin��mBox.mMax�ɐݒ肳���
		// ���_1�������̐������J��Ԃ�
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++)
		{
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
		}
	}
	mRadius = Math::Sqrt(mRadius); // 2�������Ă���

	// Load in the indices
	const rapidjson::Value& indJson = doc["indices"];
	if (!indJson.IsArray() || indJson.Size() < 1)
	{
		SDL_Log("Mesh %s ���C���f�b�N�X�������Ă��܂���", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3); // �O�p�`�Ȃ̂�3
	for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
	{
		const rapidjson::Value& ind = indJson[i];
		if (!ind.IsArray() || ind.Size() != 3)
		{
			SDL_Log("Invalid indices for %s", fileName.c_str());
			return false;
		}
		indices.emplace_back(ind[0].GetUint());
		indices.emplace_back(ind[1].GetUint());
		indices.emplace_back(ind[2].GetUint());
	}

	// ���_�z����쐬
	mVertexArray = new VertexArray(
		vertices.data(),
		static_cast<unsigned>(vertices.size()) / vertSize, // ���_�̐�
		indices.data(),
		static_cast<unsigned>(indices.size())
	);
	return true;
}

void Mesh::Unload()
{
	delete mVertexArray;
	mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index)
{
	// �͈͊O�A�N�Z�X��h��
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	else
	{
		return nullptr;
	}
}
