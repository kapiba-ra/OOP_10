#include "EnemyActor.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "InputSystem.h"

#include "ChaseMove.h"
//#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "BoxComponent.h"
#include "TargetComponent.h"
#include "HpComponent.h"
#include "DamageComponent.h"
#include "JumpComponent.h"

#include "PlayerActor.h"
#include "PlaneActor.h"
#include "ExpActor.h"

EnemyActor::EnemyActor(Game* game)
	: Actor(game, Type::Eenemy)
	, mMyState(MyState::EAlive)
	, mInvincibleDuration(0.5f)	// 無敵時間,初期設定は0.5秒
	, mInvincibilityTimer(0.0f)
	, mMaxJumpSpeed(500.0f)
{
	mMeshComp = new SkeletalMeshComponent(this);
	// 人型
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/GP_Human.gpmesh");
	mMeshComp->SetMesh(mesh);
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/GP_Human.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/GP_HumanWalk.gpanim"));
	// コーギーテスト用
	//Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Corgi_backup.gpmesh");
	//mMeshComp->SetMesh(mesh);
	//mMeshComp->SetSkeleton(game->GetSkeleton("Assets/Corgi_backup.gpskel"));
	//mMeshComp->PlayAnimation(game->GetAnimation("Assets/Corgi_walk.gpanim"));

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
	// 人型
	SetScale(10.0f);
	// コーギー
	SetScale(50.0f);

	mMyMove = new ChaseMove(this, game->GetPlayer());
	// 常に進み続ける
	mMyMove->SetForwardSpeed(150.0f);

	mJumpComp = new JumpComponent(this);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);

	// Damage(攻撃力),Hp共にデフォルトの,1
	mDamageComp = new DamageComponent(this);
	mHpComp = new HpComponent(this, 2.0f);

	// レーダーへの表示に使う
	new TargetComponent(this);
}

void EnemyActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mInvincibilityTimer += deltaTime;	// 無敵時間用のタイマーはここで加算し続ける

	if (mMyState == MyState::EDying)
	{
		float scale = GetScale() - 1.0f;
		SetScale(scale);
		if (scale <= 0.0f)
		{
			SetState(EDead);
			// アイテムを落とす
			Actor* a = new ExpActor(GetGame());
			a->SetPosition(GetPosition());
		}
	}
	// 倒された瞬間の処理
	else if (mHpComp->IsKilled() && mMyState == MyState::EAlive)
	{
		mMyState = MyState::EDying;
		mMyMove->SetForwardSpeed(0.0f);
	}

	FixCollisions();
}

void EnemyActor::Reset()
{
	// シンプルに消す(準備をする)。
	SetState(EDead);
}

void EnemyActor::FixCollisions()
{
	// 丸ごとPlayerActorにおいてもいいかも
	ComputeWorldTransform();

	const AABB& enemyBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	Vector3 offset(0, 0, 50.0f);
	LineSegment line((pos + offset), pos + Vector3::UnitZ * -100.0f);
	bool inAir(true); // 空中にいるか
	float t = 0.0f;				 // 引数設定用に便宜上必要,使わない
	Vector3 norm(Vector3::Zero); // 同上

	auto player = GetGame()->GetPlayer();
	const AABB& playerBox = player->GetBox()->GetWorldBox();
	// playerがactiveだったらという条件を追加、もっと良い感じにはできそう
	if (Intersect(enemyBox, playerBox) && player->GetState() == EActive)
	{
		// 敵の位置をプレイヤーから少し離す
		
		// プレイヤーのHPをへらす
		player->GetHpComp()->TakeDamage(1.0f);
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
				if (dz == dz1)
				{
					if (mJumpComp->IsJumping())
					{
						mJumpComp->Land();
					}
				}
			}
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
			// もし足場との衝突で,自分の足元が足場よりも少し下にある場合にジャンプ
			if ((pa->GetCategory() == PlaneActor::Category::EScaffold) &&
				(dz1>5.0f))
			{
				if (!mJumpComp->IsJumping())
				{
					mJumpComp->Liftoff(mMaxJumpSpeed);
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

void EnemyActor::TakeDamage(float amount)
{
	if (mInvincibilityTimer >= mInvincibleDuration)
	{
		mHpComp->TakeDamage(amount);
		mInvincibilityTimer = 0.0f;
	}
}

void EnemyActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
