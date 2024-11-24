#include "SwordActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

#include "MeshComponent.h"
#include "BallMove.h"
#include "DamageComponent.h"

SwordActor::SwordActor(Game* game)
	: WeaponActor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Sword.gpmesh");
	mc->SetMesh(mesh);
	mMyMove = new BallMove(this);
}

void SwordActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.0f)
	{
		SetState(EDead);
	}
}

void SwordActor::SetShotSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
