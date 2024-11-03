#include "ExpActor.h"
#include "Game.h"
#include "Renderer.h"
//#include "InputSystem.h"
#include "Mesh.h"

#include "PlayerActor.h"

#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"

ExpActor::ExpActor(Game* game)
	: ItemActor(game)
	, mExpAmount(1.0f)
{
	MeshComponent* mc = new MeshComponent(this);
	
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Exp.gpmesh");
	mc->SetMesh(mesh);
	// ‰Šú’l
	SetPosition(Vector3(2000.0f, 2000.0f, 0.0f));
	SetScale(15.0f);

	mMoveComp = new MoveComponent(this);
	mMoveComp->SetAngularSpeed(Math::Pi);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);
}

void ExpActor::OnAcquired()
{
	ItemActor::OnAcquired();

	PlayerActor* player = GetGame()->GetPlayer();
	player->GainExp(mExpAmount);
}
