#pragma once
#include "Actor.h"

/*
*  ���˂���e�̃A�N�^�[
*/

class BallActor :
    public Actor
{
public:
    BallActor(class Game* game);

    void UpdateActor(float deltaTime) override;
    void SetPlayer(Actor* player);

    void HitTarget(Actor* target);

private:
    class AudioComponent* mAudioComp;
    class BallMove* mMyMove;
    class DamageComponent* mAttackComp;
    float mLifeSpan;
};

