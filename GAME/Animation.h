#pragma once
#include "BoneTransform.h"
#include <vector>
#include <string>

/// <summary>
/// ロードしたアニメーションデータを格納するためのクラス
/// </summary>
class Animation
{
public:
	// アニメーションデータをJSONファイルから読み込む関数
	bool Load(const std::string& fileName);

	size_t GetNumBones() const { return mNumBones; }
	size_t GetNumFrames() const { return mNumFrames; }
	float GetDuration() const { return mDuration; }
	float GetFrameDuration() const { return mFrameDuration; }

	// 渡された配列にグローバルポーズ行列を記入する関数
	// 指定された時刻における,各ボーンの現在のグローバルポーズの行列
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses,
		const class Skeleton* inSkeleton,
		float inTime) const;

private:
	/* 格納するデータ達 */
	size_t mNumBones;		// アニメーションのボーン数(skeletonと異なることもある)
	size_t mNumFrames;		// アニメーションのフレーム数
	float mDuration;		// アニメーションの長さ(秒単位)
	float mFrameDuration;	// アニメーションの各フレームの長さ
	// 各フレームの変換情報をトラックに格納
	// 外側の配列のインデックスはボーン,内側のインデックスはフレームを指す
	std::vector<std::vector<BoneTransform>> mTracks;
};

// グローバルポーズは、ゲームワールドではなく
// オブジェクト空間での位置や回転の情報のこと