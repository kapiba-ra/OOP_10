#include "SwordComponent.h"
#include "Game.h"
#include "Math.h"

#include "SwordActor.h"
#include "PlayerActor.h"

SwordComponent::SwordComponent(Actor* owner)
	: WeaponComponent(owner)
	, mLastShot(0.0f)
	, mNum(2)
{
	Reset();
}

void SwordComponent::Update(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mInterval * mIntervalFactor)
	{
		mLastShot -= mInterval * mIntervalFactor;
		// �o���ʒu�̐ݒ��shotComponent�Ɠ��l,���̂Ƃ���
		Vector3 start = mOwner->GetPosition();	// �e�̔��˒n�_
		if (mOwner->GetType() == Actor::Eplayer)
		{
			start = static_cast<PlayerActor*>(mOwner)->GetBoneWorldPosition("pelvis");
		}
		Vector3 dir = mOwner->GetForward();
		float shotAngle = Math::Pi / 6;
		//dir = Vector3::Transform(dir, Quaternion(Vector3::UnitY, -shotAngle * (mNum - 1) / 2));
		 //Quaternion::Concatenate(Quaternion(Vector3::UnitX, -Math::PiOver2), )
		for (int i = 0; i < mNum; ++i)
		{
			SwordActor* sword = new SwordActor(mOwner->GetGame());
			sword->SetScale(mScale * mSizeFactor);
			Vector3 SwordOffset = dir * 100.0f;
			sword->SetPosition(start + SwordOffset);
			sword->SetPivotAndRadius(mOwner, SwordOffset);
			sword->SetRotationSpeed(Math::PiOver2 / 6 * mSpeedFactor);

			/* ���̌��������肵�Ă���...���̃��f����Z��+�������X��+�ɐn�������Ă��� */ 
			// 1:����Q������]
			Quaternion first = Quaternion(Vector3::UnitY, Math::PiOver2);
			// 2:����Owner�̐��ʕ����֌������]
			sword->RotateToNewForward(dir);
			Quaternion second = sword->GetRotation();
			// 3: 1->2�̏��ɉ�]������
			sword->SetRotation(Quaternion::Concatenate(first, second));

			dir = Vector3::Transform(dir, Quaternion(Vector3::UnitZ, shotAngle));
		}
	}
}

void SwordComponent::LevelUp(int curLv)
{
	// LevelUp�̓��e
	switch (curLv)
	{
	case 2:
	{
		mInterval *= 0.9f;
		break;
	}
	case 3:
	{
		mScale += 2.0f;
		break;
	}
	case 4:
	{
		mNum += 1;
		break;
	}
	case 5:
	{
		mScale += 2.0f;
		mInterval *= 0.9f;
		break;
	}
	}
}

void SwordComponent::Reset()
{
	mNum = 1;
	mScale = 10.0f;
	mInterval = 2.0f;
	mSpeed = Math::Pi;
}
