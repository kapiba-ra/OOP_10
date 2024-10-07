#pragma once
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

/*
*  床・壁のアクター
*/

class PlaneActor :
    public Actor
{
public:
    PlaneActor(class Game* game);
    ~PlaneActor();
    class BoxComponent* GetBox() { return mBox; }

private:
    class BoxComponent* mBox;
};

