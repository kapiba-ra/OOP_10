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
		// �e�̔��˒n�_�́AOwner��Position����z��������50��̏�����Ƃ���
		Vector3 start = mOwner->GetPosition() + Vector3(0.0f, 0.0f, 100.0f);
		// �ȉ��̗l�ɐ��m�ɍ��̈ʒu�𓾂邱�Ƃ��ł��邪,�����R�X�g�������肻��
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
			// �����o��������
			SwordActor* sword = new SwordActor(mOwner->GetGame());
			sword->SetScale(mScale * mSizeFactor);
			Vector3 SwordOffset = dir * 100.0f;
			sword->SetPosition(start + SwordOffset);
			sword->SetPivotAndRadius(mOwner, SwordOffset);
			sword->SetRotationSpeed(mSpeed * mSpeedFactor);

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

void SwordComponent::LevelUp(int preLv)
{
	// LevelUp�̓��e
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
