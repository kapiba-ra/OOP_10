#include "ShotComponent.h"
#include "Game.h"
#include "Math.h"

#include "BallActor.h"

ShotComponent::ShotComponent(Actor* owner)
	: Component(owner)
	, mShotNum(1)
	, mShotInterval(1.0f)
	, mLastShot(0.0f)
	, mBallScale(1.0f)
{
}

void ShotComponent::Update(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mShotInterval)
	{
		mLastShot -= mShotInterval;
		/* ’e‚ÌˆÊ’uA•ûŒü‚ðŒˆ‚ß‚é */
		Vector3 start = mOwner->GetPosition();
		Vector3 dir = mOwner->GetForward();
		float shotAngle = Math::Pi / 6;
		dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, -shotAngle * (mShotNum - 1) / 2));
		for (int i = 0; i < mShotNum; ++i)
		{
			// ’e‚ðì¬‚µ‚ÄFXÝ’è
			BallActor* ball = new BallActor(mOwner->GetGame());
			ball->SetPlayer(mOwner);
			ball->SetScale(mBallScale);
			ball->SetPosition(start + dir * 50.0f);
			ball->RotateToNewForward(dir);
			dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, shotAngle));
		}
	}
}
