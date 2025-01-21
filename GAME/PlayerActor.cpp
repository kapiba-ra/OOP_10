#include "PlayerActor.h"
#include <algorithm>
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"
#include "SkillSystem.h"

#include "HUD.h"
//#include "LevelUpMenu.h"

#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "MoveComponent.h"
#include "JumpComponent.h"
#include "HpComponent.h"
#include "BoxComponent.h"
#include "FollowCamera.h"
#include "ShotComponent.h"
#include "SwordComponent.h"
#include "AudioComponent.h"

#include "PlaneActor.h" // for collision caluculation
#include "BallActor.h"
#include "ItemActor.h"

#include "Skeleton.h"
#include "Animation.h"

PlayerActor::PlayerActor(Game* game)
	: Actor(game, Type::Eplayer)
	, mMoving(false)
{	
	mHUD = GetGame()->GetHUD();

	mMeshComp = new SkeletalMeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/CatWarrior.gpmesh");
	mMeshComp->SetMesh(mesh);
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/CatWarrior.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/CatActionIdle.gpanim"), 1.0f, 0.5f);
	SetPosition(Vector3(0.0f, 0.0f, -50.0f));

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);

	mMoveComp = new MoveComponent(this);
	
	mJumpComp = new JumpComponent(this);

	float initialHp = 100.0f;
	mHpComp = new HpComponent(this, initialHp);

	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
	
	//mBoxComp = new BoxComponent(this);
	//AABB myBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
	//mBoxComp->SetObjectBox(myBox);
	//mBoxComp->SetShouldRotate(false);
	
	// 初期武器設定,名前をここで指定する(武器全ての名前はSkillSystemのInitialize()で知れる)
	game->GetSkillSystem()->SetInitialWeapon("Gun", this);

	mAudioComp = new AudioComponent(this);
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::ActorInput(const InputState& state)
{
	float forwardSpeed = 0.0f;	// 前進速度
	float angularSpeed = 0.0f;	// 回転速度
	float jumpSpeed = 0.0f;		// ジャンプ速度
	// 前後・回転
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_W))
	{
		forwardSpeed += mParams.maxForwardSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_S))
	{
		forwardSpeed -= mParams.maxForwardSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_A))
	{
		angularSpeed -= Math::Pi;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_D))
	{
		angularSpeed += Math::Pi;
	}
	// ジャンプ
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_SPACE))
	{
		if (!mJumpComp->IsJumping())
		{
			mJumpComp->Liftoff(mParams.maxJumpSpeed);
		}
	}

	if (!mMoving && !Math::NearZero(forwardSpeed))
	{
		mMoving = true;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/CatRunSprint.gpanim"), 1.25f, 0.5f);
	}
	else if (mMoving && Math::NearZero(forwardSpeed))
	{
		mMoving = false;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/CatActionIdle.gpanim"), 1.0f, 0.5f);
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);

	/* カメラの設定 */
	// スピードによって水平距離を変えてスピード感を出す
	if (!Math::NearZero(forwardSpeed))
	{
		mCameraComp->SetHorzDist(500.0f);
	}
	else
	{
		mCameraComp->SetHorzDist(350.0f);
	}
	if (state.Keyboard.GetKeyState(SDL_SCANCODE_C) == ButtonState::EPressed)
	{
		mCameraComp->SwitchCameraPos();
	}
}

void PlayerActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

/*	Vector3 bonePosition = mMeshComp->GetBonePosition("pelvis");
	Matrix4 worldTransform = GetWorldTransform();
	Vector3::Transform(bonePosition, worldTransform)*/;

	CheckLevelUp();

	if (mHpComp->IsKilled())
	{
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
}

void PlayerActor::Reset()
{
	Actor::SetState(EActive);
	mParams.Reset();
	mHpComp->Reset(100.0f);
	
	// 武器をリセットする
	for (auto weapon : mWeapons)
	{
		// 親が持ってる配列からWeaponComponentを消して
		Actor::RemoveComponent(weapon.second);
		// メモリを解放
		delete weapon.second;
	}
	mWeapons.clear();
	auto skillSystem = GetGame()->GetSkillSystem();
	if (skillSystem)	// 安全運転でnullチェック
	{
		skillSystem->SetInitialWeapon("Gun", this);
	}

	SetPosition(Vector3(0.0f, 0.0f, -50.0f));
}

void PlayerActor::FixCollisions()
{
	// ワールド空間のボックスを更新するために、自分の
	// ワールド変換を再計算する必要がある
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	Vector3 offset(0, 0, 50.0f);
	// 地上から落下への遷移で使う
	// Playerのポジションが足元なので,胴体付近からにするためにoffsetを追加
	LineSegment line((pos+offset), pos + Vector3::UnitZ * -100.0f);
	bool inAir(true); // 空中にいるか
	float t = 0.0f;				 // 引数設定用に便宜上必要,使わない
	Vector3 norm(Vector3::Zero); // 同上

	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		// このPlaneActorと衝突するか
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			// 全ての差を計算する
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			// dxにはdx1,dx2のうち絶対値が小さい方をセットする
			// dy,dzも同様
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			// x,y,zのうち最も差が小さい軸で位置を調整する
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
				// 床との衝突であるとき
				if (dz == dz1) 
				{
					if (mJumpComp->IsJumping())
					{
						mJumpComp->Land();
					}
				}
			}
			// 位置を設定しボックスの成分を更新する
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
			// ここは落下用の処理
			// TODO: playerの中心から一本だけのlineでテストしているので,ステージをいじった際修正が必要そう
			if (!mJumpComp->IsJumping())
			{
				if (Intersect(line, planeBox, t, norm))
				{
					inAir = false;
				}
			}
		}
	}

	// 一定落下したらgameoverへ
	if (pos.z <= -300.0f)
	{
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
	// inAirならEfallingへ
	else if (inAir && !mJumpComp->IsJumping())
	{
		mJumpComp->Liftoff(0.0f);
	}

	// アイテム取得用
	auto& items = GetGame()->GetItems();
	for (auto item : items)
	{
		Vector3 itemPos = item->GetPosition();
		Vector3 dir = itemPos - pos;
		float getRadius = 200.0f;
		if (dir.Length() < getRadius)
		{
			item->OnAcquired();
		}
	}
}

void PlayerActor::GainExp(float exp)
{
	mParams.exp += exp;
}

//void PlayerActor::GainHeart(float recover)
//{
//	float diff = mParams.maxHp - mParams.hp;
//	//float diff = mParams.maxHp - mHpComp->GetHp();	
//	
//	if (diff < recover)
//	{
//		recover = diff;
//	}
//	mParams.hp += recover;
//}

void PlayerActor::AddWeapon(std::string name, WeaponComponent* weapon)
{
	mWeapons[name] = weapon;
}

void PlayerActor::LevelUpWeapon(std::string name, int preLv)
{
	mWeapons[name]->LevelUp(preLv);
}

float PlayerActor::GetForwardSpeed()
{
	return mMoveComp->GetForwardSpeed();
}

void PlayerActor::ApplyWeaponFactors()
{
	for (auto weapon : mWeapons)
	{
		weapon.second->SetIntervalFactor(mParams.WeaponIntervalFactor);
		weapon.second->SetSizeFactor(mParams.WeaponSizeFactor);
		weapon.second->SetSpeedFactor(mParams.WeaponSpeedFactor);
	}
}

Vector3 PlayerActor::GetBoneWorldPosition(std::string boneName)
{
	Vector3 retVec(Vector3::Zero);
	Vector3 bonePosition = mMeshComp->GetBonePosition(boneName);
	Matrix4 worldTransform = GetWorldTransform();
	// ワールドへ変換する
	retVec = Vector3::Transform(bonePosition, worldTransform);
	return retVec;
}

float PlayerActor::GetWeaponIntervalRate(std::string weaponName)
{
	// mWeaponsは武器名とWeaponComponent*のマップ
	auto it = mWeapons.find(weaponName);
	// 武器が見つかった場合
	if (it != mWeapons.end())
	{
		return it->second->GetIntervalRate();
	}
	else
	{
		// 見つからなかった場合
		return 1.0f;
	}
}

//void PlayerActor::RemoveWeapon(WeaponComponent* weapon)
//{
//	// バグのこってるよ
//	// mapなのに値の情報だけで削除したい人
//	for (auto iter = mWeapons.begin(); iter != mWeapons.end();)
//	{
//		if (iter->second == weapon)
//		{
//			iter = mWeapons.erase(iter); // 削除後、新しいイテレータを返す
//			break; // 一つだけ削除して終了
//		}
//		else
//		{
//			++iter; // 一致しない場合は次の要素へ進む
//		}
//	}
//	//auto iter = mWeapons.find(name);
//	//if (iter != mWeapons.end())
//	//{
//	//	mWeapons.erase(iter);
//	//}
//	//Actor::RemoveComponent(weapon);
//}

void PlayerActor::CheckLevelUp()
{
	if (mParams.exp > mParams.expToLevelUp)
	{
		mParams.level += 1;
		mParams.exp -= mParams.expToLevelUp;
		mParams.expToLevelUp += 1.0f;
		GetGame()->ChangeState(Game::ELevelUp);
		mAudioComp->PlayEvent("event:/Ding");
	}
}

void PlayerActor::Parameters::Reset()
{
	maxForwardSpeed = 400.0f;
	maxJumpSpeed = 700.0f;
	//hp = 100.0f;
	exp = 0.0f;
	expToLevelUp = 1.0f;
	level = 1;
}
