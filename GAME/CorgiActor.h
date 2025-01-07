#pragma once
#include "WeaponActor.h"

/// <summary>
/// •Ší:ƒR[ƒM[
/// “G‚ğ’Ç‚¢‚©‚¯‚ÄUŒ‚‚·‚é
/// </summary>
class CorgiActor :
    public WeaponActor
{
public:
    CorgiActor(class Game* game, class EnemyActor* target);

    void UpdateActor(float deltaTime) override;

    void FixCollision();

    void SetTarget(class EnemyActor* target);
    void SetSpeed(float speed);

private:
    class ChaseMove* mMyMove;       // “G‚ªg‚Á‚Ä‚éChaseMove‚ğ—¬—p‚·‚é
    class BoxComponent* mBoxComp;
    class EnemyActor* mTarget;
    class JumpComponent* mJumpComp;
};
