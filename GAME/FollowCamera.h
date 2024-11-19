#pragma once
#include "CameraComponent.h"

/// <summary>
/// 三人称視点の追従カメラを実現するComponentクラス
/// </summary>
class FollowCamera :
    public CameraComponent
{
public:
    FollowCamera(class Actor* owner);

    void Update(float deltaTime) override;

    void SnapToIdeal();

    void SetHorzDist(float dist) { mHorzDist = dist; }
    void SetVertDist(float dist) { mVertDist = dist; }
    void SetTargetDist(float dist) { mTargetDist = dist; }
    void SetSpringConstant(float spring) { mSpringConstant = spring; }

    enum CameraState
    {
        EBack,
        EAbove
    };
    void SwitchCameraPos();
    

private:
    CameraState mCameraState;

    Vector3 ComputeCameraPos() const;
    /**
    * @brief CameraとPlayerとの間に障害物(壁のアクター)があればカメラ位置を修正する
    */
    void CheckObstacles();

    Vector3 mActualPos; // カメラの実際の位置
    Vector3 mVelocity;  // 実際のカメラの速度

    float mHorzDist;       // 水平距離
    float mVertDist;       // 垂直距離
    float mTargetDist;     // ターゲット距離
    float mSpringConstant; // ばね定数(ばねの硬さを表現)
};

