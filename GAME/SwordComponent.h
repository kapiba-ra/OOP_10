#pragma once
#include "WeaponComponent.h"
#include <utility>
#include <vector>

class SwordComponent :
    public WeaponComponent
{
public:
    SwordComponent(class Actor* owner);

    void Update(float deltaTime) override;

    void LevelUp(int preLv) override;
    void Reset() override;

    // getter,setter
    int GetNum() const { return mNum; }
    float GetInterval() const { return mInterval; }
    float GetScale() const { return mScale; }
    void SetNum(int i) { mNum = i; }
    void SetInterval(float f) { mInterval = f; }
    void SetScale(float f) { mScale = f; }


private:
    // Œ•‚Æ‰ñ“]Šp“x‚Ìpair
    std::vector<std::pair<class SwordActor*, float>> mSwords;

    int mNum;
    float mInterval;
    float mScale;
    float mSpeed;

    float mLastShot;
};

