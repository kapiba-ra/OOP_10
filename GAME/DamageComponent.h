#pragma once
#include "Component.h"

/// <summary>
/// ダメージを与える仕組みを担当する
/// 攻撃力に当たるパラメータを持つ
/// </summary>
class DamageComponent :
    public Component
{
public:
    DamageComponent(class Actor* owner);
    void Damage(class Actor* target);

    void SetAttackDamage(float damage) { mDamageAmt = damage; }

private:
    float mDamageAmt;    // 攻撃力！持たせるactorによって変更可能()
};

// 最初はDamageAmtをアクター自身に持たせていたがComponentに入れれそうなら
// その方がすっきりするので入れた