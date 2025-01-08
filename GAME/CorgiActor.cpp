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
#include "JumpComponent.h"

#include "EnemyActor.h"
#include "PlaneActor.h"


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

	// jump
	mJumpComp = new JumpComponent(this);

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
	Vector3 pos = GetPosition();
	Vector3 offset(0, 0, 50.0f);
	LineSegment line((pos + offset), pos + Vector3::UnitZ * -100.0f);
	bool inAir(true); // 空中にいるか
	float t = 0.0f;				 // 引数設定用に便宜上必要,使わない
	Vector3 norm(Vector3::Zero); // 同上


	// 敵との衝突
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

	// 壁・床との衝突
	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(corgiBox, planeBox))
		{
			float dx1 = planeBox.mMax.x - corgiBox.mMin.x;
			float dx2 = planeBox.mMin.x - corgiBox.mMax.x;
			float dy1 = planeBox.mMax.y - corgiBox.mMin.y;
			float dy2 = planeBox.mMin.y - corgiBox.mMax.y;
			float dz1 = planeBox.mMax.z - corgiBox.mMin.z;
			float dz2 = planeBox.mMin.z - corgiBox.mMax.z;

			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
				if (dz == dz1)
				{
					if (mJumpComp->IsJumping())
					{
						mJumpComp->Land();
					}
				}
			}
			SetPosition(pos);
			// わずかに位置をずらすので一応変えておく
			mBoxComp->OnUpdateWorldTransform();
			// もし足場との衝突で,自分の足元が足場よりも少し下にある場合にジャンプ
			if ((pa->GetCategory() == PlaneActor::Category::EScaffold) &&
				(dz1 > 5.0f))
			{
				if (!mJumpComp->IsJumping())
				{
					mJumpComp->Liftoff(500.0f);
				}
			}
			if (!mJumpComp->IsJumping())
			{
				if (Intersect(line, planeBox, t, norm))
				{
					inAir = false;
				}
			}
		}
	}

	if (inAir && !mJumpComp->IsJumping())
	{
		// 踏切速度0でジャンプさせる(自由落下)
		mJumpComp->Liftoff(0.0f);
	}
}

void CorgiActor::SetTarget(EnemyActor* target)
{
	mTarget = target;
}

void CorgiActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
