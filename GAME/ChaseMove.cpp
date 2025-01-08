#include "ChaseMove.h"

#include "Actor.h"
#include "Game.h"
#include "Collision.h"
#include "PhysWorld.h"
#include "AStar.h"

#include "PlayerActor.h"
#include "PlaneActor.h"

#include "BoxComponent.h" // for planes box

ChaseMove::ChaseMove(Actor* owner, Actor* target)
	: MoveComponent(owner)
	, mChaseState(ETargeting)
	, mInterval(0.0f)
	, mNextPoint(Vector3::Zero)
	, mTarget(target)
{
}

void ChaseMove::Update(float deltaTime)
{
	mInterval += deltaTime;

	// mChaseStateを変更すべきかどうか判断する
	CheckObstacle(deltaTime);

	Vector3 targetPos(Vector3::Zero);
	if(mTarget) targetPos = mTarget->GetPosition();
	if (mChaseState == ETargeting)
	{
		TurnTo(targetPos);
	}
	else if (mChaseState == ESearching)
	{
		Vector3 myPos = mOwner->GetPosition();
		// 10秒待って再計算する
		if (mInterval >= 10.0f)
		{
			mInterval = 0.0f;
			mPath.clear();

			Vector3 myPos = mOwner->GetPosition();

			AStarMap map;
			WeightedGraph* g = mOwner->GetGame()->GetGraph();
			// それぞれtarget,ownerに最も近いノードを取得する
			WeightedGraphNode* myNode = FindClosestNode(*g, myPos);
			WeightedGraphNode* targetNode = FindClosestNode(*g, targetPos);
			bool found = AStarSearch(*g, myNode, targetNode, map);
			// startからgoalまでの順番に格納されている
			mPath = ReconstructPathCoords(myNode, targetNode, map);
			if (!mPath.empty())  // 空でないことを確認
			{
				mNextPoint = mPath.back();
			}
		}

		// 到達したら、次のポイントへ向けて向きを変える...を繰り返す
		Vector2 nextPoint2D(mNextPoint.x, mNextPoint.y);
		Vector2 myPos2D(myPos.x, myPos.y);
		Vector3 diff3D = mNextPoint - myPos;
		Vector2 diff2D = nextPoint2D - myPos2D;
		if (diff2D.LengthSq() < 10000.0f)
		{
			if (!mPath.empty())
			{
				mPath.pop_back();
			}
			if (!mPath.empty())  // 空でないことを確認
			{
				mNextPoint = mPath.back();
			}
		}
		// 次のポイントを向く
		TurnTo(mNextPoint);
	}

	MoveComponent::Update(deltaTime);
}

void ChaseMove::TurnTo(const Vector3& pos)
{
	// 親クラスのメンバ変数であるmAngularSpeedの制御を行う
	Vector3 dir = Vector3(pos - mOwner->GetPosition());
	dir.z = 0.0f;	// z成分は考えない
	dir.Normalize();
	Vector3 forward = mOwner->GetForward();
	float dot = Vector3::Dot(forward, dir);
	Vector3 cross = Vector3::Cross(forward, dir); // 正なら右回転すべき

	// 大体同じ方向を見てるなら(Targetingである事が前提)
	if (dot > 0.95f)
	{
		SetAngularSpeed(0.0f);
	}
	else if (cross.z > 0.0f)
	{
		SetAngularSpeed(Math::Pi);
	}
	else if (cross.z <= 0.0f)
	{
		SetAngularSpeed(-Math::Pi);
	}
}

void ChaseMove::SetTarget(Actor* target)
{
	mTarget = target;
	mPath.clear();
	mInterval = 10.0f;
}

void ChaseMove::CheckObstacle(float deltaTime)
{
	/* 問題：PlaneActorが線分の間にあるかの判定の際に,床のPlaneActorを判定
	* することがある。解決策は2つ,一つは線分の始点を床よりちょっと上にすること,
	* もう一つは,PlaneActorが床Actorなら..という条件分岐をすること
	* 前者の方がわだかまりが少なそうなので,前者を採用している
	*/

	ChaseState stateBeforeCheck = mChaseState;
	// playerまでの間の線分を作る
	// posが足元だと床と接触してる判定になることがあり厄介なので,z方向に0.1プラスしている
	Vector3 pos = mOwner->GetPosition() + Vector3(0.0f, 0.0f, 0.1f);
	Vector3 targetPos(Vector3::Zero);
	if(mTarget) targetPos = mTarget->GetPosition();
	// targetPosは大体足元で、胴体付近の方が都合がいいのでz方向に+100.0fしている
	LineSegment line(pos, targetPos);

	// 床と壁ひっくるめて(今は床の分が無駄だが,いずれ2階建てにも対応できるかも)
	auto& planes = mOwner->GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		float t = 0.0f;
		Vector3 norm(Vector3::Zero);
		const AABB& planeBox = pa->GetBox()->GetWorldBox();

		// もしもplayerとの間に壁か足場があったら,Searching状態へ
		// Floorかどうかの判定は行っていない。
		if ((Intersect(line, planeBox, t, norm))) //&&
			//(pa->GetCategory() != (PlaneActor::Category::EFloor)))
		{
			mChaseState = ESearching;
			if(stateBeforeCheck == ETargeting)
			{
				OnEnter();
			}
			// 何かにぶつかっていたら処理が終わる
			return;
		}
		//PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
		//PhysWorld::CollisionInfo info;
		//phys->TestSweepAndPrune([](Actor* a, Actor* b) {
		//	// タグで条件分岐
		//	if (a->GetType() == Actor::Type::Eplane)
		//	{
		//		static_cast<PlaneActor*>(a);
		//	}
		//});
	}


	// 何事もなかったらETargetingへ
	mChaseState = ETargeting;
}

void ChaseMove::OnEnter()
{
	switch (mChaseState)
	{
	case ETargeting:
	{
		break;
	}
	case ESearching:
	{
		mInterval = 10.0f;
		break;
	}
	}
}
