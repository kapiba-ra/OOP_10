#include "SwordActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PhysWorld.h"
#include "Actor.h"
#include "EnemyActor.h"

#include "MeshComponent.h"
#include "SwordMove.h"
#include "DamageComponent.h"
#include "BoxComponent.h"

SwordActor::SwordActor(Game* game)
	: WeaponActor(game)
	, mPivotActor(nullptr)
{
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
	// PivotActor not null‚Å
	if (mPivotActor)
	{
		Vector3 pivot = mPivotActor->GetPosition();
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
		// sword‚Æenemy‚ªŒð·‚µ‚Ä‚¢‚éê‡‚Ìˆ—
		if (sword == this) // SwordActor ‚ªŒð·‚µ‚Ä‚¢‚éê‡
		{
			// enemy‚ªEnemyActor‚Ìê‡(Type‚Å”»’f)
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
