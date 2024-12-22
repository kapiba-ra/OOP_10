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

    enum class UniState // UniqueState�̗�(Actor���Ƌ�ʂ���)
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
    float mInvincibleDuration;  // ���G����
    float mInvincibilityTimer;  // ���G���ԗp�̃^�C�}�[

    class ChaseMove* mMyMove;
    class JumpComponent* mJumpComp;
    float mMaxJumpSpeed;
    class SkeletalMeshComponent* mMeshComp;
    class BoxComponent* mBoxComp;
    class DamageComponent* mDamageComp;
    class HpComponent* mHpComp;
};

