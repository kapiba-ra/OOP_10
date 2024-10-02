#pragma once
#include "CameraComponent.h"

class FPSCamera :
    public CameraComponent
{
public:
    FPSCamera(class Actor* owner);

    void Update(float deltaTime) override;

    float GetPitch() const { return mPitch; }
    float GetPitchSpeed() const { return mPitchSpeed; }
    float GetMaxPitch() const { return mMaxPitch; }

    void SetPitchSpeed(float speed) { mPitchSpeed = speed; }
    void SetMaxPitch(float pitch) { mMaxPitch = pitch; }

private:
    float mPitchSpeed;  // ピッチの角速度
    float mMaxPitch;    // 最大ピッチ角度
    float mPitch;       // 現在のピッチ
};

