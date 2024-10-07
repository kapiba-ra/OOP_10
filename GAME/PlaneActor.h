#pragma once
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

/*
*  ���E�ǂ̃A�N�^�[
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

