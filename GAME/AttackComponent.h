#pragma once
#include "Component.h"

class AttackComponent :
    public Component
{
public:
    AttackComponent(class Actor* owner);
    void Damage(class Actor* target);

    void SetAttackDamage(float damage) { mAttackDamage = damage; }

private:
    float mAttackDamage;    // 攻撃力！持たせるactorによって変更可能()
};

// 最初はAttackDamageをアクター自身に持たせていたがComponentに入れれそうなら
// その方がすっきりするので入れた