#pragma once
#include "Actor.h"

/// <summary>
/// ����̊��N���X
/// </summary>
class WeaponActor :
    public Actor
{
public:
    WeaponActor(class Game* game);
    virtual ~WeaponActor();

    void Reset() override;
    
    void HitTarget(Actor* target);

protected:
    class DamageComponent* mDamageComp;
    float mLifeSpan;
};
