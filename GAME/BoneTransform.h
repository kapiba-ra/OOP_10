#pragma once
#include "Math.h"

/// <summary>
/// ボーンの位置と回転を保存
/// </summary>
class BoneTransform
{
public:
	Quaternion mRotation;
	Vector3 mTranslation;
	// シェーダーに渡すのは行列なので,行列への変換が必要
	Matrix4 ToMatrix() const;
	// アニメーションのフレーム間を補完するための関数
	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};

