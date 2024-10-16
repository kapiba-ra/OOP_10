#include "ItemActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"

ItemActor::ItemActor(Game* game)
	: Actor(game, Type::Eitem)
{
	MeshComponent* mc = new MeshComponent(this);
	// ‰¼
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Cube.gpmesh");
	mc->SetMesh(mesh);
	SetPosition(Vector3(2000.0f, 2000.0f, 0.0f));
	SetScale(15.0f);

	mMoveComp = new MoveComponent(this);
	mMoveComp->SetAngularSpeed(Math::Pi);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);
}

void ItemActor::Reset()
{
	SetState(EDead);
}
