#include "EnemyActor.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

//#include "MoveComponent.h"
#include "ChaseMove.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "TargetComponent.h"

#include "PlayerActor.h"
#include "PlaneActor.h"
// ui

EnemyActor::EnemyActor(Game* game)
	: Actor(game, Type::Eenemy)
	, mMoveComp(nullptr)
	, mMeshComp(nullptr)
	, mBoxComp(nullptr)
	, mHP(1.0f)
	, mTimer(0.0f)
{
	mMeshComp = new MeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Human.gpmesh");
	mMeshComp->SetMesh(mesh);

	// TODO: ランダムspawnにしたい。Gameクラスで設定
	WeightedGraph* g = game->GetGraph();
	size_t size = g->mNodes.size();
	std::random_device rd;    // シードを生成
	std::mt19937 gen(rd());   // メルセンヌ・ツイスタ乱数生成器
	std::uniform_int_distribution<> dist(0, size - 1);  // 0から99の範囲で乱数を生成
	int randomIndex = dist(gen);
	WeightedGraphNode* node = g->mNodes[randomIndex];
	if (node->type != NodeType::ENoAccess)
	{
		SetPosition(node->NodePos);
	}
	
	// メッシュによる
	SetScale(40.0f);

	mMyMove = new ChaseMove(this);
	// 常に進み続ける
	mMyMove->SetForwardSpeed(100.0f);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);

	// レーダーへの表示に使う
	new TargetComponent(this);
}

void EnemyActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	if (mHP <= 0.0f)
	{
		SetState(EDead);
	}

	FixCollisions();
	
}

void EnemyActor::Reset()
{
	// シンプルに消す(準備をする)。
	// SetState(EDead);
}

void EnemyActor::FixCollisions()
{
	// 丸ごとPlayerActorにおいてもいいかも
	ComputeWorldTransform();

	const AABB& enemyBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	auto player = GetGame()->GetPlayer();
	const AABB& playerBox = player->GetBox()->GetWorldBox();
	// playerがactiveだったらという条件を追加、もっと良い感じにはできそう
	if (Intersect(enemyBox, playerBox) && player->GetState() == EActive)
	{
		// 敵の位置をプレイヤーから少し離す
		
		// プレイヤーのHPをへらす
		player->TakeDamage(1.0f);
	}

	// 壁・床との衝突
	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(enemyBox, planeBox))
		{
			float dx1 = planeBox.mMax.x - enemyBox.mMin.x;
			float dx2 = planeBox.mMin.x - enemyBox.mMax.x;
			float dy1 = planeBox.mMax.y - enemyBox.mMin.y;
			float dy2 = planeBox.mMin.y - enemyBox.mMax.y;
			float dz1 = planeBox.mMax.z - enemyBox.mMin.z;
			float dz2 = planeBox.mMin.z - enemyBox.mMax.z;

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
			}
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}
