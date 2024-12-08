#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxComponent.h"

/* Base */
PlaneActor::PlaneActor(Game* game, Category category)
	: Actor(game, Type::Eplane)
	, mCategory(category)
{
	SetScale(5.0f); // c‰¡500‚Ì’·‚³‚É‚È‚Á‚Ä‚é‚Í‚¸

	/* MeshComponent */
	mMeshComp = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Plane.gpmesh");
	mMeshComp->SetMesh(mesh);

	/* BoxComponent */
	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());

	game->AddPlane(this);
}

PlaneActor::~PlaneActor()
{
	GetGame()->RemovePlane(this);
}

/* Floor */
FloorActor::FloorActor(Game* game)
	: PlaneActor(game, Category::EFloor)
{
	mMeshComp->SetTextureIndex(0);
}

WallActor::WallActor(Game* game)
	: PlaneActor(game, Category::EWall)
{
	SetScale(10.0f);
	mMeshComp->SetTextureIndex(1);
}

ScaffoldActor::ScaffoldActor(Game* game)
	: PlaneActor(game, Category::EScaffold)
{
	mMeshComp->SetTextureIndex(2);
}
