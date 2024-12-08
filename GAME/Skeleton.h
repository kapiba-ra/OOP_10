#pragma once
#include  "BoneTransform.h"
#include <vector>
#include <string>

/// <summary>
/// ���[�h�����X�P���g���f�[�^���i�[���邽�߂̃N���X
/// �L�����N�^�[�̃X�P���g���̓{�[���̊K�w�\���ŕ\�������
/// </summary>
class Skeleton
{
public:
	// �X�P���g���ɂ���X�̃{�[���̒�`
	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;	// �e�{�[���̃C���f�b�N�X
	};

	// �X�P���g���̓t�@�C������ǂݍ���
	bool Load(const std::string& fileName);

	/* Getter */ 
	// �{�[���̐���Ԃ�
	size_t GetNumBones() const { return mBones.size(); }
	// �C���f�b�N�X�w�肳�ꂽ�{�[����Ԃ�
	const Bone& GetBone(size_t idx) const { return mBones[idx]; }
	// �S�{�[����const�Q�Ƃ�Ԃ�
	const std::vector<Bone>& GetBones() const { return mBones; }
	// �t�o�C���h�|�[�Y�s��(T�̎��|�[�Y�ɂ�����e�{�[���̃��[�J�����(�e����̑��Έʒu�E��])
	// ����AT�̎��|�[�Y�ɂ�����I�u�W�F�N�g��Ԃł̈ʒu�E��]�ɖ߂����߂̍s��)��Ԃ�
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const { return mGlobalInvBindPoses; }
	
protected:
	// �S�Ẵ{�[����(�O���[�o����)�t�o�C���h�|�[�Y�s����v�Z����
	// �{�[�������[�h���I�������A�X�P���g�����[�h���ɌĂяo�����
	void ComputeGlobalInvBindPose();
private:
	// �S�Ẵ{�[���̔z��,���ԑ厖�Ȃ̂œ���ւ����Ⴂ���Ȃ�
	std::vector<Bone> mBones;
	// �e�{�[����(�O���[�o����)�t�o�C���h�|�[�Y�s����i�[����z��
	std::vector<Matrix4> mGlobalInvBindPoses;
};

