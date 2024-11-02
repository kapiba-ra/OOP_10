#include "ItemActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"

#include "PlayerActor.h"

ItemActor::ItemActor(Game* game)
	: Actor(game, Type::Eitem)
{
	MeshComponent* mc = new MeshComponent(this);
	// ��
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Exp.gpmesh");
	mc->SetMesh(mesh);
	SetPosition(Vector3(2000.0f, 2000.0f, 0.0f));
	SetScale(15.0f);

	mMoveComp = new MoveComponent(this);
	mMoveComp->SetAngularSpeed(Math::Pi);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);

	game->AddItem(this);
}

ItemActor::~ItemActor()
{
	GetGame()->RemoveItem(this);
}

void ItemActor::Reset()
{
	SetState(EDead);
}

void ItemActor::OnAcquired()
{
	SetState(EDead);

	// Player�Ɏ擾���̏���(����exp�A�C�e�������ɂ��Ă�)
	PlayerActor* player = GetGame()->GetPlayer();
	float expAmount = 1.0f; // EnemyActor������ݒ�ł���悤�ɂ�����
	player->GainExp(expAmount);
}
