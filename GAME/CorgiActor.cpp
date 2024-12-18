#include "CorgiActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PhysWorld.h"

#include "Skeleton.h"
#include "Animation.h"

#include "SkeletalMeshComponent.h"
#include "ChaseMove.h"
#include "DamageComponent.h"
#include "BoxComponent.h"

#include "EnemyActor.h"


CorgiActor::CorgiActor(Game* game, EnemyActor* target)
	: WeaponActor(game)
	, mTarget(target)
{
	// mesh
	SkeletalMeshComponent* mc = new SkeletalMeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Corgi_backup.gpmesh");
	mc->SetMesh(mesh);
	mc->SetSkeleton(game->GetSkeleton("Assets/Corgi_backup.gpskel"));
	mc->PlayAnimation(game->GetAnimation("Assets/Corgi_walk.gpanim"));

	// move
	mMyMove = new ChaseMove(this, mTarget);
	// collision
	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(true);

	// 攻撃力高め
	mDamageComp->SetAttackDamage(5.0f);
}


	void CorgiActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollision();

	// ターゲットが有効じゃなくなったorターゲットが消えた->ターゲットを更新する
	if ((!mTarget) || mTarget->GetUniState() == EnemyActor::UniState::EDying)
	{
		auto& enemies = GetGame()->GetEnemies();
		// この配列から、UniState::Aliveのものを選ぶようにしたい
		for (auto enemy : enemies)
		{
			if (enemy->GetUniState() == EnemyActor::UniState::EAlive)
			{
				mTarget = enemy; // ターゲットを設定
				mMyMove->SetTarget(mTarget);
				return; // 最初に見つけた敵をターゲットにして終了
			}
		}
		// 条件に合う敵がいなければ、ターゲットを nullptr に設定
		mTarget = nullptr;
		mMyMove->SetTarget(nullptr);
	}
}

void CorgiActor::FixCollision()
{
	ComputeWorldTransform();

	const AABB& corgiBox = mBoxComp->GetWorldBox();

	GetGame()->GetPhysWorld()->TestSweepAndPrune([this, corgiBox](Actor* corgi, Actor* enemy)
	{
		if (corgi == this) // CorgiActor が交差している場合
		{
			// enemyがEnemyActorの場合(Typeで判断)
			if (enemy->GetType() == Eenemy && enemy->GetState() == EActive)
			{
				// 敵にヒット
				HitTarget(enemy);
			}
		}
	});
}

void CorgiActor::SetTarget(EnemyActor* target)
{
	mTarget = target;
}

void CorgiActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
