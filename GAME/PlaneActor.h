#pragma once
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

/*
*  ����(���E�ǁE����)��Actor�̊��N���X
*/

class PlaneActor :
    public Actor
{
public:
    // �qActor�̎�ނ�ݒ�(���Actor�Ƌ�ʂ��ăJ�e�S���Ƃ������O�ɂ���)
    enum class Category
    {
        EFloor,
        EWall,
        EScaffold
    };
   
    PlaneActor(class Game* game, Category category);
    virtual ~PlaneActor();

    class BoxComponent* GetBox() { return mBoxComp; }
    Category GetCategory() const { return mCategory; }

private:
    Category mCategory;

protected:
    class BoxComponent* mBoxComp;
    class MeshComponent* mMeshComp;
};

class FloorActor :
    public PlaneActor
{
public:
    FloorActor(class Game* game);
};

class WallActor :
    public PlaneActor
{
public:
    WallActor(class Game* game);
};

class ScaffoldActor :
    public PlaneActor
{
public:
    ScaffoldActor(class Game* game);
};