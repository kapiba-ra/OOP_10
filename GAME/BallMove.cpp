#include "BallMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "BallActor.h"

BallMove::BallMove(Actor* owner)
	: MoveComponent(owner)
{
}

void BallMove::Update(float deltaTime)
{
	// 進行方向の線分を構築
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();	// 始点
	Vector3 dir = mOwner->GetForward();		// 進行方向
	Vector3 end = start + dir * segmentLength; // 終点
	LineSegment ls(start, end);

	// 線分とワールドの衝突を判定
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// Player,Weaponとは衝突しないようにしている
	if (phys->SegmentCast(ls, info) && 
		(info.mActor->GetType() == Actor::Eenemy ||
		info.mActor->GetType() == Actor::Eplane) 
		)
	{
		// 衝突したら法線の向きで方向を反射させる
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);
		/* ターゲットにヒットしたか */
		if (info.mActor->GetType() == Actor::Type::Eenemy)
		{
			mOwner->SetState(Actor::State::EDead); // erase ball
			static_cast<BallActor*>(mOwner)->HitTarget(info.mActor);
		}
	}

	// 基底クラスで動きを更新する
	MoveComponent::Update(deltaTime);
}

// SegmentCastは線分とワールドの全てのボックスとのテストを行う
// 元の設計では当たったボックスの持ち主を知るために,
// info.mActorを派生クラスのポインタにdynamic_castしていた。
// ただ,RTTIに依存せずに設計するように,基底クラスに派生クラスの情報を
// Typeとして持たせ,Typeから判断できるようにした。