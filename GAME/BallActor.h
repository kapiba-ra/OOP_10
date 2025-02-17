#pragma once
#include "WeaponActor.h"

/// <summary>
/// 発射する弾のアクター
/// </summary>
class BallActor :
    public WeaponActor
{
public:
    BallActor(class Game* game);

    void UpdateActor(float deltaTime) override;

    //void HitTarget(Actor* target);
    void SetShotSpeed(float speed);

private:
    class AudioComponent* mAudioComp;
    class BallMove* mMyMove;
    //class DamageComponent* mAttackComp;
};

