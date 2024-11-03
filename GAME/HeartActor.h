#pragma once
#include "ItemActor.h"

class HeartActor :
    public ItemActor
{
public:
    HeartActor(class Game* game);

    void OnAcquired() override;

private:
    float mRecoverAmount;
};

