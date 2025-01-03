#include "BallActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"

#include "MeshComponent.h"
#include "BallMove.h"

BallActor::BallActor(Game* game)
	: WeaponActor(game)
{
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh");
	mc->SetMesh(mesh);
	mMyMove = new BallMove(this);

	//mAttackComp = new DamageComponent(this);
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

//void BallActor::HitTarget(Actor* target)
//{
//	mAttackComp->Damage(target);
//}

void BallActor::SetShotSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
