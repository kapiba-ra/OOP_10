#pragma once
#include  "BoneTransform.h"
#include <vector>
#include <string>

/// <summary>
/// ロードしたスケルトンデータを格納するためのクラス
/// キャラクターのスケルトンはボーンの階層構造で表現される
/// </summary>
class Skeleton
{
public:
	// スケルトンにある個々のボーンの定義
	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;	// 親ボーンのインデックス
	};

	// スケルトンはファイルから読み込む
	bool Load(const std::string& fileName);

	/* Getter */ 
	// ボーンの数を返す
	size_t GetNumBones() const { return mBones.size(); }
	// インデックス指定されたボーンを返す
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	// 全ボーンのconst参照を返す
	const std::vector<Bone>& GetBones() const { return mBones; }
	// 逆バインドポーズ行列(Tの字ポーズにおける各ボーンのローカル空間(親からの相対位置・回転)
	// から、Tの字ポーズにおけるオブジェクト空間での位置・回転に戻すための行列)を返す
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
	
protected:
	// 全てのボーンの(グローバルな)逆バインドポーズ行列を計算する
	// ボーンをロードし終わった後、スケルトンロード時に呼び出される
	void ComputeGlobalInvBindPose();
private:
	// 全てのボーンの配列,順番大事なので入れ替えちゃいけない
	std::vector<Bone> mBones;
	// 各ボーンの(グローバルな)逆バインドポーズ行列を格納する配列
	std::vector<Matrix4> mGlobalInvBindPoses;
};

