#include "SwordComponent.h"
#include "Game.h"
#include "Math.h"

#include "SwordActor.h"
#include "PlayerActor.h"

SwordComponent::SwordComponent(Actor* owner)
	: WeaponComponent(owner)
	, mLastShot(0.0f)
{
	Reset();
}

void SwordComponent::Update(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mInterval * mIntervalFactor)
	{
		mLastShot -= mInterval * mIntervalFactor;
		// 弾の発射地点は、OwnerのPositionからz軸方向に50上の所からとする
		Vector3 start = mOwner->GetPosition() + Vector3(0.0f, 0.0f, 100.0f);
		// 以下の様に正確に腰の位置を得ることができるが,少しコストがかかりそう
		//if (mOwner->GetType() == Actor::Eplayer)
		//{
		//	start = static_cast<PlayerActor*>(mOwner)->GetBoneWorldPosition("pelvis");
		//}
		Vector3 dir = mOwner->GetForward();
		float shotAngle = Math::Pi / 6;
		//dir = Vector3::Transform(dir, Quaternion(Vector3::UnitY, -shotAngle * (mNum - 1) / 2));
		 //Quaternion::Concatenate(Quaternion(Vector3::UnitX, -Math::PiOver2), )
		for (int i = 0; i < mNum; ++i)
		{
			// 剣を出現させる
			SwordActor* sword = new SwordActor(mOwner->GetGame());
			sword->SetScale(mScale * mSizeFactor);
			Vector3 SwordOffset = dir * 100.0f;
			sword->SetPosition(start + SwordOffset);
			sword->SetPivotAndRadius(mOwner, SwordOffset);
			sword->SetRotationSpeed(mSpeed * mSpeedFactor);

			/* 剣の向きを決定している...剣のモデルはZ軸+が剣先でX軸+に刃が向いてる状態 */ 
			// 1:剣を寝かす回転
			Quaternion first = Quaternion(Vector3::UnitY, Math::PiOver2);
			// 2:剣をOwnerの正面方向へ向ける回転
			sword->RotateToNewForward(dir);
			Quaternion second = sword->GetRotation();
			// 3: 1->2の順に回転させる
			sword->SetRotation(Quaternion::Concatenate(first, second));

			dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, shotAngle));
		}
	}
}

void SwordComponent::LevelUp(int preLv)
{
	// LevelUpの内容
	switch (preLv)
	{
	case 2:
	{
		mSpeed *= 1.1f;
		mInterval *= 0.9f;
		break;
	}
	case 3:
	{
		mScale *= 1.1f;
		break;
	}
	case 4:
	{
		mNum += 1;
		break;
	}
	case 5:
	{
		mScale *= 0.9f;
		mInterval *= 0.8f;
		break;
	}
	}
}

void SwordComponent::Reset()
{
	mNum = 1;
	mScale = 10.0f;
	mInterval = 4.0f;
	mSpeed = Math::PiOver2;
}

float SwordComponent::GetIntervalRate()
{
	return mLastShot / (mIntervalFactor * mInterval);
}
