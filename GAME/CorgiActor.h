#pragma once
#include "WeaponActor.h"

/// <summary>
/// 武器:コーギー
/// 敵を追いかけて攻撃する
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
    class ChaseMove* mMyMove;       // 敵が使ってるChaseMoveを流用する
    class BoxComponent* mBoxComp;
    class EnemyActor* mTarget;
    class JumpComponent* mJumpComp;
};
