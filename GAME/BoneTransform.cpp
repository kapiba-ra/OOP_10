#include "BoneTransform.h"

Matrix4 BoneTransform::ToMatrix() const
{
	Matrix4 rot = Matrix4::CreateFromQuaternion(mRotation);
	Matrix4 trans = Matrix4::CreateTranslation(mTranslation);
	// ��]->�I�u�W�F�N�g���W
	return rot * trans;
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	// a�͑O�̃t���[���̃{�[���ϊ�,b�͎��̃t���[���̃{�[���ϊ�,f�͕⊮�W��(0-1�����҂����)
	BoneTransform retVal;
	// �u���ʐ��`�⊮�v�Ȃ���̂��s��
	retVal.mRotation = Quaternion::Slerp(a.mRotation, b.mRotation, f);
	// ���`�⊮���s��
	retVal.mTranslation = Vector3::Lerp(a.mTranslation, b.mTranslation, f);
	// ���[�J���|�[�Y��Ԃ�
	return retVal;

	// ���`�⊮�ɂ��āA���Ƃ���
	// a.mTranslation...(10, 0, 0)
	// b.mTranslation...(0, 0, 0)
	// f...0.25  �Ȃ��
	// retVal.mTranslation...(2.5, 0, 0)�ƂȂ�B
	// Rotation�̕���Quaternion�Ȃ̂ŋ�̗�ōl���Â炢��,
	// ���������悤�Ȃ��Ƃ����Ă���
}
