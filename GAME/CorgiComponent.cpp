#include "CorgiComponent.h"
#include "Game.h"
#include "Math.h"

#include "Actor.h"
#include "CorgiActor.h"
#include "EnemyActor.h"

CorgiComponent::CorgiComponent(Actor* owner)
	: WeaponComponent(owner)
	, mTargetActive(false)
{
	Reset();
	
	// ownerのPosに,ownerの向きを向いて生まれる
	Vector3 spawnPos = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	auto& enemies = mOwner->GetGame()->GetEnemies();
	EnemyActor* target = nullptr;
	if (enemies.size() != 0)
	{
		mTargetActive = true;
		target = enemies[0];
	}
	// 以降はCorgiActor側でターゲットの設定をしてもらう
	mCorgi = new CorgiActor(mOwner->GetGame(), target);
	// 大きくなるのは変&&床の下で大きくなると挟まっちゃうのでScaleFactorは考慮しない。
	mCorgi->SetScale(mScale);
	mCorgi->SetPosition(spawnPos);
	mCorgi->SetSpeed(mSpeed);

}

void CorgiComponent::LevelUp(int preLv)
{
	switch (preLv)
	{
	case 1:
	{
		mSpeed *= 1.1f;
		mCorgi->SetSpeed(mSpeed * mSpeedFactor);
		break;
	}
	case 2:
	{
		mSpeed *= 1.1f;
		mCorgi->SetSpeed(mSpeed * mSpeedFactor);
		break;
	}
	case 3:
	{
		mSpeed *= 1.1f;
		mCorgi->SetSpeed(mSpeed * mSpeedFactor);
		break;
	}
	case 4:
	{
		mSpeed *= 1.2f;
		mCorgi->SetSpeed(mSpeed * mSpeedFactor);
		break;
	}
	// MaxLv5なので終了
	}
}

void CorgiComponent::Reset()
{
	mScale = 50.0f;
	mSpeed = 300.0f;
}
