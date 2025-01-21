#pragma once
#include "WeaponComponent.h"

/// <summary>
/// コーギー生成用のコンポーネント
/// 生成するアクターは永続の1つなので,ポインタを持たせた
/// </summary>
class CorgiComponent :
    public WeaponComponent
{
public:
    CorgiComponent(class Actor* owner);

    void LevelUp(int preLv) override;
    void Reset() override;
    float GetIntervalRate() override { return 1.0f; }
private:
    class CorgiActor* mCorgi;

    bool mTargetActive; // ターゲットが生きているか

    float mScale;
    float mSpeed;
};

