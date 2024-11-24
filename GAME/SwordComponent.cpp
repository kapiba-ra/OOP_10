#include "SwordComponent.h"
#include "Game.h"
#include "Math.h"

#include "SwordActor.h"

SwordComponent::SwordComponent(Actor* owner)
	: WeaponComponent(owner)
	, mNum(1)
	, mInterval(2.0f)
	, mLastShot(0.0f)
	, mScale(10.0f)
	, mSpeed(1000.0f)
{
}

void SwordComponent::Update(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mInterval)
	{
		mLastShot -= mInterval;
		Vector3 start = mOwner->GetPosition();
		Vector3 dir = mOwner->GetForward();
		float shotAngle = Math::Pi / 6;
		dir = Vector3::Transform(dir, Quaternion(Vector3::UnitY, -shotAngle * (mNum - 1) / 2));
		for (int i = 0; i < mNum; ++i)
		{
			SwordActor* sword = new SwordActor(mOwner->GetGame());
			sword->SetScale(mScale);
			sword->SetPosition(start + dir * 20.0f);
			sword->RotateToNewForward(dir);
			sword->SetShotSpeed(mSpeed);
			dir = Vector3::Transform(dir, Quaternion(Vector3::UnitY, shotAngle));
		}
	}
}

void SwordComponent::Reset()
{
	mScale = 1.0f;
	mNum = 1;
	mInterval = 2.0f;
	mSpeed = 1000.0f;
}
