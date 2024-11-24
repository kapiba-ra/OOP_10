#pragma once
#include "WeaponActor.h"

/// <summary>
/// 武器:ソード
/// SwordMoveを作成する必要がある
/// </summary>
class SwordActor :
    public WeaponActor
{
public:
    SwordActor(class Game* game);

    void UpdateActor(float deltaTime) override;

    void SetShotSpeed(float speed);

private:
    class BallMove* mMyMove;
    //class AudioComponent* mAudioComp;
};


