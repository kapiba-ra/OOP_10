#include "SwordActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PhysWorld.h"
#include "Actor.h"
#include "EnemyActor.h"
#include "PlayerActor.h"

#include "MeshComponent.h"
#include "SwordMove.h"
#include "DamageComponent.h"
#include "BoxComponent.h"

SwordActor::SwordActor(Game* game)
	: WeaponActor(game)
	, mPivotActor(nullptr)
{
	mLifeSpan = 1.0f;	// 短めに

	// mesh
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Sword.gpmesh");
	mc->SetMesh(mesh);
	// move
	mMyMove = new SwordMove(this);
	// collision
	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(true);
}

void SwordActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.0f)
	{
		SetState(EDead);
	}
	// PivotActor not nullで
	if (mPivotActor)
	{
		Vector3 pivot = mPivotActor->GetPosition();
		if (mPivotActor->GetType() == Actor::Eplayer)
		{
			pivot = static_cast<PlayerActor*>(mPivotActor)->GetBoneWorldPosition("pelvis");
		}
		mMyMove->SetPivot(pivot);
	}
	
	FixCollision();
}

void SwordActor::FixCollision()
{
	ComputeWorldTransform();

	const AABB& swordBox = mBoxComp->GetWorldBox();

	GetGame()->GetPhysWorld()->TestSweepAndPrune([this, swordBox](Actor* sword, Actor* enemy)
	{
		// swordとenemyが交差している場合の処理
		if (sword == this) // SwordActor が交差している場合
		{
			// enemyがEnemyActorの場合(Typeで判断)
			if (enemy->GetType() == Eenemy && enemy->GetState() == EActive)
			{
				HitTarget(enemy);
			}
		}
	});
}

void SwordActor::SetShotSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}

void SwordActor::SetRotationSpeed(float speed)
{
	mMyMove->SetAngularSpeed(speed);
}

void SwordActor::SetPivotAndRadius(Actor* actor, const Vector3& offset)
{
	mPivotActor = actor;
	Vector3 pivot = mPivotActor->GetPosition();
	mMyMove->SetPivot(pivot);
	mMyMove->SetOffset(offset);
}
