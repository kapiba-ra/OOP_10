#pragma once
#include "BoneTransform.h"
#include <vector>
#include <string>

/// <summary>
/// ���[�h�����A�j���[�V�����f�[�^���i�[���邽�߂̃N���X
/// </summary>
class Animation
{
public:
	// �A�j���[�V�����f�[�^��JSON�t�@�C������ǂݍ��ފ֐�
	bool Load(const std::string& fileName);

	size_t GetNumBones() const { return mNumBones; }
	size_t GetNumFrames() const { return mNumFrames; }
	float GetDuration() const { return mDuration; }
	float GetFrameDuration() const { return mFrameDuration; }

	// �n���ꂽ�z��ɃO���[�o���|�[�Y�s����L������֐�
	// �w�肳�ꂽ�����ɂ�����,�e�{�[���̌��݂̃O���[�o���|�[�Y�̍s��
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses,
		const class Skeleton* inSkeleton,
		float inTime) const;

private:
	/* �i�[����f�[�^�B */
	size_t mNumBones;		// �A�j���[�V�����̃{�[����(skeleton�ƈقȂ邱�Ƃ�����)
	size_t mNumFrames;		// �A�j���[�V�����̃t���[����
	float mDuration;		// �A�j���[�V�����̒���(�b�P��)
	float mFrameDuration;	// �A�j���[�V�����̊e�t���[���̒���
	// �e�t���[���̕ϊ������g���b�N�Ɋi�[
	// �O���̔z��̃C���f�b�N�X�̓{�[��,�����̃C���f�b�N�X�̓t���[�����w��
	std::vector<std::vector<BoneTransform>> mTracks;
};

// �O���[�o���|�[�Y�́A�Q�[�����[���h�ł͂Ȃ�
// �I�u�W�F�N�g��Ԃł̈ʒu���]�̏��̂���