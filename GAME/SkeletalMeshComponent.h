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
    // 返り値はアニメーションの長さ
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f);
    // 指定のボーンのオブジェクト空間での位置を返す
    class Vector3 GetBonePosition(std::string boneName);

private:
    void ComputeMatrixPalette();
    const class Skeleton* mSkeleton;
    const class Animation* mAnimation; // 再生中のアニメーション
    MatrixPalette mPalette;      // 行列パレット
    float mAnimPlayRate;         // アニメーションの再生速度
    float mAnimTime;             // アニメーションの現在の時刻
    std::vector<class Matrix4> mCurrentPoses;
};

