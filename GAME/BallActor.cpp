#include "BallActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

#include "MeshComponent.h"
#include "AudioComponent.h"
#include "BallMove.h"
#include "AttackComponent.h"

BallActor::BallActor(Game* game)
	: Actor(game, Type::Eball)
	, mLifeSpan(2.0f)
{
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh");
	mc->SetMesh(mesh);
	mMyMove = new BallMove(this);
	mMyMove->SetForwardSpeed(1500.0f);
	mAudioComp = new AudioComponent(this);

	mAttackComp = new AttackComponent(this);
}

void BallActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.0f)
	{
		SetState(EDead);
	}
}

void BallActor::SetPlayer(Actor* player)
{
	mMyMove->SetPlayer(player);
}

void BallActor::HitTarget(Actor* target)
{
	mAudioComp->PlayEvent("event:/Ding");
	mAttackComp->Damage(target);
}
