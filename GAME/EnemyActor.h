#pragma once
#include "Actor.h"

class EnemyActor :
    public Actor
{
public:
    EnemyActor(class Game* game);

    void UpdateActor(float deltaTime) override;
    void Reset() override;

    void FixCollisions();

    void TakeDamage(float amount) { mHP -= amount; }

private:
    float mHP;
    float mTimer;

    class MoveComponent* mMoveComp;
    class ChaseMove* mMyMove;
    class MeshComponent* mMeshComp;
    class BoxComponent* mBoxComp;

};

