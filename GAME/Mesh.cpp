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
	/* gpmeshファイルから情報を読み込んでいく */

	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("ファイルが見つかりません : %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	// RapidJSONで解析する為、用意されたstreamを経由する
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Mesh %s は無効なjsonです", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	if (ver != 1)
	{
		SDL_Log("Mesh %s のバージョンが1ではありません", fileName.c_str());
		return false;
	}

	mShaderName = doc["shader"].GetString();

	size_t vertSize = 8; // 頂点1つに含まれる情報の数

	const rapidjson::Value& textures = doc["textures"];
	if (!textures.IsArray() || textures.Size() < 1)
	{
		SDL_Log("Mesh %s がテクスチャを持っていません", fileName.c_str());
		return false;
	}

	mSpecPower = static_cast<float>(doc["specularPower"].GetFloat());

	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// テクスチャは既にLoadされているか
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);
		if (t == nullptr)
		{
			// 再ロードを試みる
			t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// 指定のテクスチャがなくても動くように
				t = renderer->GetTexture("Assets/Default.png");
			}
		}
		mTextures.emplace_back(t);
	}

	// Load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		SDL_Log("Mesh % s が頂点を持っていません", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	vertices.reserve(vertsJson.Size() * vertSize);	// 静的に確保しておく	
	mRadius = 0.0f;
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// 要素数 : 8
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray() || vert.Size() != 8)
		{
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}
		// 各頂点は8つの要素を持ち,最初の3要素はx,y,z座標を表していた
		Vector3 pos(vert[0].GetFloat(), vert[1].GetFloat(), vert[2].GetFloat());
		mRadius = Math::Max(mRadius, pos.LengthSq());
		mBox.UpdateMinMax(pos);	// mBox.mMinとmBox.mMaxに設定される
		// 頂点1つが持つ情報の数だけ繰り返す
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++)
		{
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
		}
	}
	mRadius = Math::Sqrt(mRadius); // 2乗を取っておく

	// Load in the indices
	const rapidjson::Value& indJson = doc["indices"];
	if (!indJson.IsArray() || indJson.Size() < 1)
	{
		SDL_Log("Mesh %s がインデックスを持っていません", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;
	indices.reserve(indJson.Size() * 3); // 三角形なので3
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

	// 頂点配列を作成
	mVertexArray = new VertexArray(
		vertices.data(),
		static_cast<unsigned>(vertices.size()) / vertSize, // 頂点の数
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
	// 範囲外アクセスを防ぐ
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	else
	{
		return nullptr;
	}
}
