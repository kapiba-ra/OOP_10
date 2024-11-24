#pragma once
#include "WeaponActor.h"

/// <summary>
/// ����:�\�[�h
/// SwordMove���쐬����K�v������
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


