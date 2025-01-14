#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"
#include <string>
#include <vector>

class SkeletalMeshComponent :
    public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    void Draw(class Shader* shader) override;

    void Update(float deltaTime) override;

    void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }
    // �Ԃ�l�̓A�j���[�V�����̒���
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f, float blendTime = 0.0f);
    // �w��̃{�[���̃I�u�W�F�N�g��Ԃł̈ʒu��Ԃ�
    class Vector3 GetBonePosition(std::string boneName);

private:
    void ComputeMatrixPalette();
    MatrixPalette mPalette;      // �s��p���b�g
    const class Skeleton* mSkeleton;
    const class Animation* mAnimation;      // �Đ����̃A�j���[�V����
    const class Animation* mPrevAnimation;  // �O�̃A�j���[�V����
    float mAnimPlayRate;         // �A�j���[�V�����̍Đ����x
    float mAnimTime;             // �A�j���[�V�����̌��݂̎���
    float mPrevAnimTime;         // �O�̃A�j���[�V�����̍Đ�����
    float mBlendTime;            // �A�j���[�V�������u�����h����
    float mBlendTimer;           // �A�j���[�V�������u�����h�p�^�C�}�[,�o�ߎ���
    std::vector<class Matrix4> mCurrentPoses;
};

