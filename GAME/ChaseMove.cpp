#include "ChaseMove.h"

#include "Actor.h"
#include "Game.h"
#include "Collision.h"
#include "AStar.h"

#include "PlayerActor.h"
#include "PlaneActor.h"

#include "BoxComponent.h" // for planes box

ChaseMove::ChaseMove(Actor* owner)
	: MoveComponent(owner)
	, mChaseState(ETargeting)
	, mInterval(0.0f)
	, mNextPoint(Vector3::Zero)
{
	// このコンポーネントをアタッチするActorよりも先に
	// Playerは生成済みでなくてはならない
	mPlayer = mOwner->GetGame()->GetPlayer();
}

void ChaseMove::Update(float deltaTime)
{
	mInterval += deltaTime;

	// mChaseStateを変更すべきかどうか判断する
	CheckPath(deltaTime);

	if (mChaseState == ETargeting)
	{
		Vector3 playerPos = mPlayer->GetPosition();
		TurnTo(playerPos);
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
			Vector3 playerPos = mPlayer->GetPosition();
			// それぞれplayer,ownerに最も近いノードを取得する
			WeightedGraphNode* myNode = FindClosestNode(*g, myPos);
			WeightedGraphNode* playerNode = FindClosestNode(*g, playerPos);
			bool found = AStarSearch(*g, myNode, playerNode, map);
			// startからgoalまでの順番に格納されている
			mPath = ReconstructPathCoords(myNode, playerNode, map);
			if (!mPath.empty())  // 空でないことを確認
			{
				mNextPoint = mPath.back();
			}
		}

		// 到達したら、次のポイントへ向けて向きを変える...を繰り返す
		Vector2 nextPoint2D(mNextPoint.x, mNextPoint.y);
		Vector2 myPos2D(myPos.x, myPos.y);
		//Vector3 diff = mNextPoint - myPos;
		Vector2 diff = nextPoint2D - myPos2D;
		if (diff.LengthSq() < 400.0f)
		{
			mPath.pop_back();
			if (!mPath.empty())  // 空でないことを確認
			{
				mNextPoint = mPath.back();
			}
		}
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

void ChaseMove::CheckPath(float deltaTime)
{
	ChaseState beforeCheck = mChaseState;
	// playerまでの間の線分を作る
	Vector3 pos = mOwner->GetPosition();
	LineSegment line(pos, mPlayer->GetPosition());

	// 床と壁ひっくるめて(今は床の分が無駄だが,いずれ2階建てにも対応できるかも)
	auto& planes = mOwner->GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		float t = 0.0f;
		Vector3 norm(Vector3::Zero);
		const AABB& planeBox = pa->GetBox()->GetWorldBox();

		// もしもplayerとの間に床か壁があったら,Searching状態へ
		if (Intersect(line, planeBox, t, norm))
		{
			mChaseState = ESearching;
			//if (mChaseState == ETargeting)
			if(beforeCheck == ETargeting)
			{
				OnEnter();
			}
			// 何かにぶつかっていたら終わる
			return;
		}
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
