#pragma once
#include "Component.h"

/// <summary>
/// アクターにHpを持たせる.
/// Component化したのは、敵・プレイヤー・破壊できるオブジェクト等にアタッチすることを想定したため
/// </summary>
class HpComponent :
    public Component
{
public:
    HpComponent(class Actor* owner, float maxHp = 1.0f);
    
    void Update(float deltaTime) override;
    void Reset(float initialHp);

    void TakeDamage(float damage);
    void Recover(float recover);

    // 0以下で
    bool IsKilled() const { return mCurHp <= 0.0f; }

    // Getter,Setter
    float GetCurHp() const { return mCurHp; }
    void SetMaxHp(float maxHp) { mMaxHp = maxHp; }
    // 便利系(SetterGetterでもできるけど...という奴)
    float GetHpPercentage() const { return mCurHp / mMaxHp; }
    void AddMaxHp(float add);

private:
    float mCurHp;
    float mMaxHp;
};

