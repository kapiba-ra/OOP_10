#pragma once
#include "Actor.h"

/*
*  発射する弾のアクター
*/

class BallActor :
    public Actor
{
public:
    BallActor(class Game* game);

    void UpdateActor(float deltaTime) override;
    void SetPlayer(Actor* player);

    void HitTarget();

private:
    class AudioComponent* mAudioComp;
    class BallMove* mMyMove;
    float mLifeSpan;
};

