#pragma once
#include "Actor.h"

// アイテムの種類が増えたら基底クラスにしたいかも

class ItemActor :
    public Actor
{
public:
    // 使うかわからないけど
    enum ItemType
    {
        Eexp,
    };

    ItemActor(class Game* game);
    ~ItemActor();
    
    void OnAcquired();
    void Reset() override;

private:

    class MoveComponent* mMoveComp;
    class BoxComponent* mBoxComp;
};

