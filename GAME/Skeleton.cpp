#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "MatrixPalette.h"

bool Skeleton::Load(const std::string& fileName)
{
	// とりあえずコピペ
	// まずはボーン配列mBonesにすべてのボーンを格納する
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Skeleton %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Skeleton %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// Check the metadata
	if (ver != 1)
	{
		SDL_Log("Skeleton %s unknown format", fileName.c_str());
		return false;
	}

	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
		return false;
	}

	size_t count = bonecount.GetUint();

	if (count > MAX_SKELETON_BONES)
	{
		SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	mBones.reserve(count);

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		SDL_Log("Skeleton %s doesn't have a bone array?", fileName.c_str());
		return false;
	}

	if (bones.Size() != count)
	{
		SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
		return false;
	}

	Bone temp;

	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		if (!bones[i].IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& name = bones[i]["name"];
		temp.mName = name.GetString();

		const rapidjson::Value& parent = bones[i]["parent"];
		temp.mParent = parent.GetInt();

		const rapidjson::Value& bindpose = bones[i]["bindpose"];
		if (!bindpose.IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		if (!rot.IsArray() || !trans.IsArray())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		temp.mLocalBindPose.mRotation.x = rot[0].GetFloat();
		temp.mLocalBindPose.mRotation.y = rot[1].GetFloat();
		temp.mLocalBindPose.mRotation.z = rot[2].GetFloat();
		temp.mLocalBindPose.mRotation.w = rot[3].GetFloat();

		temp.mLocalBindPose.mTranslation.x = trans[0].GetFloat();
		temp.mLocalBindPose.mTranslation.y = trans[1].GetFloat();
		temp.mLocalBindPose.mTranslation.z = trans[2].GetFloat();

		mBones.emplace_back(temp);
	}

	// Now that we have the bones
	// ボーン入れ終わったら,逆バインドポーズ行列を計算する
	ComputeGlobalInvBindPose();

	return true;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	// ボーンの数で配列の要素数を変更する
	mGlobalInvBindPoses.resize(GetNumBones());

	/* ステップ1: 各ボーンのグローバルバインドポーズを計算する。*/ 
	// ルートのグローバルバインドポーズだけはローカルバインドポーズと同じ
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// 残りのボーンのグローバルバインドポーズは、
	// ローカルポーズに親のグローバルポーズを掛けたもの
	// この段階でのmGlobalInvBindPosesは、グローバルバインドポーズ行列(逆じゃない)
	// が入っており、ステップ2で逆バインドポーズ行列にする手順であることに注意する
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); ++i)
	{
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
	}

	/* ステップ2: 各行列の逆行列を求める */
	// グローバルバインドポーズを逆バインドポーズ行列へ
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); ++i)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}
