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
    float PlayAnimation(const class Animation* anim, float playRate = 1.0f, float blendTime = 0.0f);
    // 指定のボーンのオブジェクト空間での位置を返す
    class Vector3 GetBonePosition(std::string boneName);

private:
    void ComputeMatrixPalette();
    MatrixPalette mPalette;      // 行列パレット
    const class Skeleton* mSkeleton;
    const class Animation* mAnimation;      // 再生中のアニメーション
    const class Animation* mPrevAnimation;  // 前のアニメーション
    float mAnimPlayRate;         // アニメーションの再生速度
    float mAnimTime;             // アニメーションの現在の時刻
    float mPrevAnimTime;         // 前のアニメーションの再生時間
    float mBlendTime;            // アニメーションをブレンドする
    float mBlendTimer;           // アニメーションをブレンド用タイマー,経過時間
    std::vector<class Matrix4> mCurrentPoses;
};

