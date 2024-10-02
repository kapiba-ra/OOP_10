#pragma once
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

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

