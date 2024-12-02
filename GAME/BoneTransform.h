#pragma once
#include "Math.h"

/// <summary>
/// ローカルボーンの位置と回転を保存(親ボーンからの相対的な座標,回転)
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

