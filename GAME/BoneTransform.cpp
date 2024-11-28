#include "BoneTransform.h"

Matrix4 BoneTransform::ToMatrix() const
{
	Matrix4 rot = Matrix4::CreateFromQuaternion(mRotation);
	Matrix4 trans = Matrix4::CreateTranslation(mTranslation);
	// 回転->オブジェクト座標
	return rot * trans;
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	// aは前のフレームのボーン変換,bは次のフレームのボーン変換,fは補完係数(0-1が期待される)
	BoneTransform retVal;
	// 「球面線形補完」なるものを行う
	retVal.mRotation = Quaternion::Slerp(a.mRotation, b.mRotation, f);
	// 線形補完を行う
	retVal.mTranslation = Vector3::Lerp(a.mTranslation, b.mTranslation, f);
	// ローカルポーズを返す
	return retVal;

	// 線形補完について、たとえば
	// a.mTranslation...(10, 0, 0)
	// b.mTranslation...(0, 0, 0)
	// f...0.25  ならば
	// retVal.mTranslation...(2.5, 0, 0)となる。
	// Rotationの方はQuaternionなので具体例で考えづらいが,
	// 多分同じようなことをしている
}
