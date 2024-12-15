#include "HeartActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

#include "MeshComponent.h"
#include "BoxComponent.h"
#include "MoveComponent.h"
#include "HpComponent.h"

#include "PlayerActor.h"

HeartActor::HeartActor(Game* game)
	: ItemActor(game)
	, mRecoverAmount(30.0f)
{
	MeshComponent* mc = new MeshComponent(this);

	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Heart.gpmesh");
	mc->SetMesh(mesh);
	SetPosition(Vector3(-2000.0f, 2000.0f, 0.0f));
	SetScale(15.0f);

	mMoveComp = new MoveComponent(this);
	mMoveComp->SetAngularSpeed(Math::Pi);

	mBoxComp = new BoxComponent(this);
	//mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);
}

void HeartActor::OnAcquired()
{
	ItemActor::OnAcquired();

	PlayerActor* player = GetGame()->GetPlayer();
	//player->GainHeart(mRecoverAmount);
	player->GetHpComp()->Recover(mRecoverAmount);
}
