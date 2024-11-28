#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "MatrixPalette.h"

bool Skeleton::Load(const std::string& fileName)
{
	// �Ƃ肠�����R�s�y
	// �܂��̓{�[���z��mBones�ɂ��ׂẴ{�[�����i�[����
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
	// �{�[������I�������,�t�o�C���h�|�[�Y�s����v�Z����
	ComputeGlobalInvBindPose();

	return true;
}

void Skeleton::ComputeGlobalInvBindPose()
{
	// �{�[���̐��Ŕz��̗v�f����ύX����
	mGlobalInvBindPoses.resize(GetNumBones());

	/* �X�e�b�v1: �e�{�[���̃O���[�o���o�C���h�|�[�Y���v�Z����B*/ 
	// ���[�g�̃O���[�o���o�C���h�|�[�Y�����̓��[�J���o�C���h�|�[�Y�Ɠ���
	mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

	// �c��̃{�[���̃O���[�o���o�C���h�|�[�Y�́A
	// ���[�J���|�[�Y�ɐe�̃O���[�o���|�[�Y���|��������
	// ���̒i�K�ł�mGlobalInvBindPoses�́A�O���[�o���o�C���h�|�[�Y�s��(�t����Ȃ�)
	// �������Ă���A�X�e�b�v2�ŋt�o�C���h�|�[�Y�s��ɂ���菇�ł��邱�Ƃɒ��ӂ���
	for (size_t i = 1; i < mGlobalInvBindPoses.size(); ++i)
	{
		Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
		mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
	}

	/* �X�e�b�v2: �e�s��̋t�s������߂� */
	// �O���[�o���o�C���h�|�[�Y���t�o�C���h�|�[�Y�s���
	for (size_t i = 0; i < mGlobalInvBindPoses.size(); ++i)
	{
		mGlobalInvBindPoses[i].Invert();
	}
}
