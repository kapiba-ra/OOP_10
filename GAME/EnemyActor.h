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
    class HpComponent* GetHpComp() { return mHpComp; }
    Vector3 GetHeadPosition();

private:
    // 倒された時のアニメーションを再生する。
    // アニメーションの名前のフォーマットがバラバラなら,この関数があった方が良さそう
    virtual void PlayDyingAnimation() {};

protected:
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

// スライム
class Slime :
    public EnemyActor
{
public:
    Slime(class Game* game);
private:
    void PlayDyingAnimation() override;
};

// ゾンビ
class Zombie :
    public EnemyActor
{
public:
    Zombie(class Game* game);
private:
    void PlayDyingAnimation() override;
};