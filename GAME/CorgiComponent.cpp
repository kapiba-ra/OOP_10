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
	
	// ownerÇÃPosÇ…,ownerÇÃå¸Ç´Çå¸Ç¢Çƒê∂Ç‹ÇÍÇÈ
	Vector3 spawnPos = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	auto& enemies = mOwner->GetGame()->GetEnemies();
	EnemyActor* target = nullptr;
	if (enemies.size() != 0)
	{
		mTargetActive = true;
		target = enemies[0];
	}
	mCorgi = new CorgiActor(mOwner->GetGame(), target);
	mCorgi->SetScale(mScale * mSizeFactor);
	mCorgi->SetPosition(spawnPos);
	mCorgi->SetSpeed(mSpeed);

}

void CorgiComponent::LevelUp(int preLv)
{
	switch (preLv)
	{
	case 1:
	{
		break;
	}
	case 2:
	{
		mScale *= 1.2f;
		break;
	}
	case 3:
	{
		mSpeed *= 1.2f;
		break;
	}
	case 4:
	{
		mScale *= 1.2f;
		mSpeed *= 1.2f;
		break;
	}
	// MaxLv5Ç»ÇÃÇ≈èIóπ
	}
}

void CorgiComponent::Reset()
{
	mScale = 50.0f;
	mSpeed = 300.0f;
}
