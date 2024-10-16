#pragma once
#include "Actor.h"

// アイテムの種類が増えたら基底クラスにしたいかも

class ItemActor :
    public Actor
{
public:
    ItemActor(class Game* game);

    void Reset() override;

private:
    class MoveComponent* mMoveComp;
    class BoxComponent* mBoxComp;
};

