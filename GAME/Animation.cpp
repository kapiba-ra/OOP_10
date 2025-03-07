#include "Animation.h"
#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>

bool Animation::Load(const std::string& fileName)
{
	// ここはコピペ(コメント,スケーリング関連以外)

	std::ifstream file(fileName);
	// ステップ1 : 指定された名前のファイルを読み込む
	if (!file.is_open())
	{
		SDL_Log("File not found: Animation %s", fileName.c_str());
		return false;
	}
	// rapidjsonで解析する為の準備
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	// JSONにおいて{}でくくられたものをオブジェクトという
	if (!doc.IsObject())
	{
		SDL_Log("Animation %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();
	// Check the metadata
	if (ver != 1)
	{
		SDL_Log("Animation %s unknown format", fileName.c_str());
		return false;
	}

	// ステップ1 : 連続したアニメーションのデータをゲットしていく
	const rapidjson::Value& sequence = doc["sequence"];
	if (!sequence.IsObject())
	{
		SDL_Log("Animation %s doesn't have a sequence.", fileName.c_str());
		return false;
	}

	const rapidjson::Value& frames = sequence["frames"];
	const rapidjson::Value& length = sequence["length"];
	const rapidjson::Value& bonecount = sequence["bonecount"];

	if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
	{
		SDL_Log("Sequence %s has invalid frames, length, or bone count.", fileName.c_str());
		return false;
	}

	// ステップ3 : アニメーションの基本情報を記録
	mNumFrames = frames.GetUint();
	mDuration = length.GetFloat();
	mNumBones = bonecount.GetUint();
	mFrameDuration = mDuration / (mNumFrames - 1);	// -1するのは,ラストのフレームが最初のフレームと同じだから

	mTracks.resize(mNumBones);

	const rapidjson::Value& tracks = sequence["tracks"];

	if (!tracks.IsArray())
	{
		SDL_Log("Sequence %s missing a tracks array.", fileName.c_str());
		return false;
	}

	// ステップ4 : 各ボーンの,アニメーションの各フレームにおけるTransformを記録していく
	// ボーンの数だけループ
	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
	{
		if (!tracks[i].IsObject())
		{
			SDL_Log("Animation %s: Track element %d is invalid.", fileName.c_str(), i);
			return false;
		}
		// "bone" : ボーンのインデックス　みたいに記録されているのでそこからとってくる
		size_t boneIndex = tracks[i]["bone"].GetUint();

		const rapidjson::Value& transforms = tracks[i]["transforms"];
		if (!transforms.IsArray())
		{
			SDL_Log("Animation %s: Track element %d is missing transforms.", fileName.c_str(), i);
			return false;
		}

		BoneTransform temp;

		if (transforms.Size() < mNumFrames)
		{
			SDL_Log("Animation %s: Track element %d has fewer frames than expected.", fileName.c_str(), i);
			return false;
		}

		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
		{
			// スケーリング(scaleがないフォーマットにも対応)
			if (transforms[j].HasMember("scale") && transforms[j]["scale"].IsArray())
			{
				const rapidjson::Value& scale = transforms[j]["scale"];
				temp.mScale.x = scale[0].GetFloat();
				temp.mScale.y = scale[1].GetFloat();
				temp.mScale.z = scale[2].GetFloat();
			}
			else
			{
				temp.mScale.x = 1.0f;
				temp.mScale.y = 1.0f;
				temp.mScale.z = 1.0f;
			}
			const rapidjson::Value& rot = transforms[j]["rot"];
			const rapidjson::Value& trans = transforms[j]["trans"];

			if (!rot.IsArray() || !trans.IsArray())
			{
				SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
				return false;
			}

			// 回転
			temp.mRotation.x = rot[0].GetFloat();
			temp.mRotation.y = rot[1].GetFloat();
			temp.mRotation.z = rot[2].GetFloat();
			temp.mRotation.w = rot[3].GetFloat();
			// 位置
			temp.mTranslation.x = trans[0].GetFloat();
			temp.mTranslation.y = trans[1].GetFloat();
			temp.mTranslation.z = trans[2].GetFloat();

			mTracks[boneIndex].emplace_back(temp);
		}
	}

	return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
    // 必要ならばoutPosesの配列の要素数を再設定する
    // アニメーションで動かすボーンの数とスケルトンが持つボーンの数が
    // 一致しない場合に行われる処理だと思われる
    if (outPoses.size() != mNumBones)
    {
        outPoses.resize(mNumBones);
    }

    // 現在と次のフレームのインデックスを求める
    // (inTimeの範囲は[0,AnimDuration]であることを期待する)
    size_t frame = static_cast<size_t>(inTime / mFrameDuration);
    size_t nextFrame = frame + 1;
    // frameとnextFrameの間の小数値を計算
    float pct = inTime / mFrameDuration - frame;

    // ルートのポーズを設定する
    // 全てのボーンがトラックを持つとは限らないのでチェック。
    if (mTracks[0].size() > 0)
    {
        // 現在のフレームのポーズと次のフレームとの間で補完をする
        BoneTransform interp = BoneTransform::Interpolate(
            mTracks[0][frame],
            mTracks[0][nextFrame], pct
        );
        // ルートのグローバルポーズ == ローカルポーズ
        outPoses[0] = interp.ToMatrix();
    }
    else
    {
        outPoses[0] = Matrix4::Identity;
    }

    const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
    // 他のすべてのボーンのグローバルポーズ行列を計算する
    for (size_t bone = 1; bone < mNumBones; ++bone)
    {
        Matrix4 localMat;
        if (mTracks[bone].size() > 0)
        {
            BoneTransform interp = BoneTransform::Interpolate(
                mTracks[bone][frame],
                mTracks[bone][nextFrame], pct
            );
            localMat = interp.ToMatrix();
        }
        outPoses[bone] = localMat * outPoses[bones[bone].mParent];
    }
}
