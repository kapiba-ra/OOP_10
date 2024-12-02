#pragma once
#include "Math.h"

/// <summary>
/// ���[�J���{�[���̈ʒu�Ɖ�]��ۑ�(�e�{�[������̑��ΓI�ȍ��W,��])
/// </summary>
class BoneTransform
{
public:
	Quaternion mRotation;
	Vector3 mTranslation;
	// �V�F�[�_�[�ɓn���͍̂s��Ȃ̂�,�s��ւ̕ϊ����K�v
	Matrix4 ToMatrix() const;
	// �A�j���[�V�����̃t���[���Ԃ�⊮���邽�߂̊֐�
	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};

