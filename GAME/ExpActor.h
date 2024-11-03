#pragma once
#include "ItemActor.h"

class ExpActor :
    public ItemActor
{
public:
    ExpActor(class Game* game);

    void OnAcquired() override;

    void SetExpAmount(float amount) { mExpAmount = amount; }
    float GetExpAmount() const { return mExpAmount; }

private:
    float mExpAmount;
};

