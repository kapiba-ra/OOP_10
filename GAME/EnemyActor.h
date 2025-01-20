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
    void SetMaxHp(float hp);
    float GetHpPercentage();
    Vector3 GetHeadPosition();

private:
    // �|���ꂽ���̃A�j���[�V�������Đ�����B
    virtual void PlayDyingAnimation() {};

protected:
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

// �X���C��
class Slime :
    public EnemyActor
{
public:
    Slime(class Game* game);
private:
    void PlayDyingAnimation() override;
};

// �]���r
class Zombie :
    public EnemyActor
{
public:
    Zombie(class Game* game);
private:
    void PlayDyingAnimation() override;
};