#pragma once
#include "WeaponActor.h"

/// <summary>
/// ����:�R�[�M�[
/// �G��ǂ������čU������
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
    class ChaseMove* mMyMove;       // �G���g���Ă�ChaseMove�𗬗p����
    class BoxComponent* mBoxComp;
    class EnemyActor* mTarget;
    class JumpComponent* mJumpComp;
};
