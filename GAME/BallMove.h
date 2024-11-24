#pragma once
#include "MoveComponent.h"

class BallMove :
    public MoveComponent
{
public:
    BallMove(class Actor* owner);

    void Update(float deltaTime) override;
};

