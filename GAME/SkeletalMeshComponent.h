#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent :
    public MeshComponent
{
public:
    SkeletalMeshComponent(class Actor* owner);
    void Draw(class Shader* shader) override;

    void Update(float deltaTime) override;

    void SetSkeleton(const class Skeleton* sk) { mSkeleton = sk; }
    // �Ԃ�l�̓A�j���[�V�����̒���
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f);

private:
    void ComputeMatrixPalette();
    const class Skeleton* mSkeleton;
    const class Animation* mAnimation; // �Đ����̃A�j���[�V����
    MatrixPalette mPalette;      // �s��p���b�g
    float mAnimPlayRate;         // �A�j���[�V�����̍Đ����x
    float mAnimTime;             // �A�j���[�V�����̌��݂̎���
};

