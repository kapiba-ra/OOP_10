#pragma once
#include "Actor.h"

/// <summary>
/// •Ší‚ÌŠî’êƒNƒ‰ƒX
/// </summary>
class WeaponActor :
    public Actor
{
public:
    WeaponActor(class Game* game);
    virtual ~WeaponActor();

    void Reset() override;
    
    void HitTarget(Actor* target);

    void SetDamageAmt(float damage);

protected:
    class DamageComponent* mDamageComp;
    float mLifeSpan;
};
