#pragma once
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"

/*
*  平面(床・壁・足場)のActorの基底クラス
*/

class PlaneActor :
    public Actor
{
public:
    // 子Actorの種類を設定(基底Actorと区別してカテゴリという名前にした)
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