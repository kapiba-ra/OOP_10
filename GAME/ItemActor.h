#pragma once
#include "Actor.h"

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
    virtual ~ItemActor();
    
    virtual void OnAcquired();
    void Reset() override;

protected:
    class MoveComponent* mMoveComp;
    class BoxComponent* mBoxComp;
};

