#include "ShotComponent.h"
#include "Game.h"
#include "Math.h"

#include "BallActor.h"
#include "PlayerActor.h"

ShotComponent::ShotComponent(Actor* owner)
	: WeaponComponent(owner)
	, mLastShot(0.0f)
{
	Reset();
}

void ShotComponent::Update(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mShotInterval * mIntervalFactor)
	{
		mLastShot -= mShotInterval * mIntervalFactor;
		/* 弾の位置、方向を決める */
		Vector3 start = mOwner->GetPosition() + Vector3(0.0f, 0.0f, 100.0f);// 弾の発射地点
		Vector3 dir = mOwner->GetForward();	// 弾の方向
		float shotAngle = Math::Pi / 6;
		dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, -shotAngle * (mShotNum - 1) / 2));
		for (int i = 0; i < mShotNum; ++i)
		{
			// 弾を作成して色々設定
			BallActor* ball = new BallActor(mOwner->GetGame());
			ball->SetScale(mBallScale * mSizeFactor);
			ball->SetPosition(start + dir * 50.0f);
			ball->RotateToNewForward(dir);
			ball->SetShotSpeed(mShotSpeed * mSpeedFactor);
			dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, shotAngle));
		}
	}
}

void ShotComponent::LevelUp(int preLv)
{
	mShotNum += 1;
	switch (preLv)
	{
	case 1:
	{
		break;
	}
	case 2:
	{
		mShotInterval *= 0.9f;
		break;
	}
	case 3:
	{
		mShotSpeed += 200.0f;
		mShotInterval *= 0.9f;
		break;
	}
	case 4:
	{
		mShotInterval *= 0.8f;
		break;
	}
	// MaxLv5なので終了
	}
}

void ShotComponent::IncShotSpeed(float add)
{
	mShotSpeed += add;
}

float ShotComponent::GetIntervalRate()
{
	return mLastShot / (mIntervalFactor * mShotInterval);
}

void ShotComponent::Reset()
{
	mBallScale = 1.0f;
	mShotNum = 1;
	mShotInterval = 1.0f;
	mShotSpeed = 1500.0f;
}
