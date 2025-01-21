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
	
	// owner��Pos��,owner�̌����������Đ��܂��
	Vector3 spawnPos = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	auto& enemies = mOwner->GetGame()->GetEnemies();
	EnemyActor* target = nullptr;
	if (enemies.size() != 0)
	{
		mTargetActive = true;
		target = enemies[0];
	}
	// �ȍ~��CorgiActor���Ń^�[�Q�b�g�̐ݒ�����Ă��炤
	mCorgi = new CorgiActor(mOwner->GetGame(), target);
	// �傫���Ȃ�͕̂�&&���̉��ő傫���Ȃ�Ƌ��܂����Ⴄ�̂�ScaleFactor�͍l�����Ȃ��B
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
	// MaxLv5�Ȃ̂ŏI��
	}
}

void CorgiComponent::Reset()
{
	mScale = 50.0f;
	mSpeed = 300.0f;
}
