#pragma once
#include "Actor.h"

class EnemyActor :
    public Actor
{
public:
    EnemyActor(class Game* game);
    virtual ~EnemyActor();

    void UpdateActor(float deltaTime) override;
    void Reset() override;

    enum class UniState // UniqueStateの略(Actor基底と区別した)
    {
        EAlive,
        EDying,
    };
    UniState GetUniState() const { return mUniState; }

    void FixCollisions();

    void TakeDamage(float amount);
    void SetSpeed(float speed);

private:
    UniState mUniState;
    float mInvincibleDuration;  // 無敵時間
    float mInvincibilityTimer;  // 無敵時間用のタイマー

    class ChaseMove* mMyMove;
    class JumpComponent* mJumpComp;
    float mMaxJumpSpeed;
    class SkeletalMeshComponent* mMeshComp;
    class BoxComponent* mBoxComp;
    class DamageComponent* mDamageComp;
    class HpComponent* mHpComp;
};

