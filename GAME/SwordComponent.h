#pragma once
#include "WeaponComponent.h"

class SwordComponent :
    public WeaponComponent
{
public:
    SwordComponent(class Actor* owner);

    void Update(float deltaTime) override;

    // getter,setter
    int GetNum() const { return mNum; }
    float GetInterval() const { return mInterval; }
    float GetScale() const { return mScale; }
    void SetNum(int i) { mNum = i; }
    void SetInterval(float f) { mInterval = f; }
    void SetScale(float f) { mScale = f; }

    void Reset();

private:
    int mNum;
    float mInterval;
    float mScale;
    float mSpeed;

    float mLastShot;
};

