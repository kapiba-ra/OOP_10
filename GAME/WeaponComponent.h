#pragma once
#include "Component.h"

/// <summary>
/// 武器をアタッチするコンポーネント.
/// 全ての武器は固有のパラメータを持つ(速度や発射間隔等)
/// </summary>
class WeaponComponent :
    public Component
{
public:
    WeaponComponent(class Actor* owner);
    ~WeaponComponent();

    //  強化内容を派生クラスで設定する,Lv2以降の強化内容を設定する
    virtual void LevelUp(int CurLv);

    void SetSizeFactor(float factor) { mSizeFactor = factor; }
    void SetIntervalFactor(float factor) { mIntervalFactor = factor; }
    void SetSpeedFactor(float factor) { mSpeedFactor = factor; }

protected:
    float mSizeFactor;
    float mIntervalFactor;
    float mSpeedFactor;
};

