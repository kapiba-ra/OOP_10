#pragma once
//#include "Component.h"
#include "WeaponComponent.h"

class ShotComponent :
    //public Component
    public WeaponComponent
{
public:
    ShotComponent(class Actor* owner);

    void Update(float deltaTime) override;

    void LevelUp(int preLv) override;
    void Reset() override;

    // getter,setter
    int GetShotNum() const { return mShotNum; }
    float GetShotInterval() const { return mShotInterval; }
    float GetBallScale() const { return mBallScale; }
    void SetShotNum(int i) { mShotNum = i; }
    void SetShotInterval(float f) { mShotInterval = f; }
    void SetBallScale(float f) { mBallScale = f; }

    void IncShotSpeed(float add);
    float GetIntervalRate() override;

private:
    int mShotNum;
    float mShotInterval;
    float mBallScale;
    float mShotSpeed;
    
    float mLastShot;
};

