#include "WeaponComponent.h"
#include "PlayerActor.h"
#include "WeaponActor.h"
#include "SkillSystem.h"

WeaponComponent::WeaponComponent(Actor* owner)
	: Component(owner)
	, mSizeFactor(1.0f)
	, mIntervalFactor(1.0f)
	, mSpeedFactor(1.0f)
{
	//if (owner->GetType() == Actor::Eplayer)
	//{
	//	static_cast<PlayerActor*>(owner)->AddWeapon(this);
	//}
}

WeaponComponent::~WeaponComponent()
{
}

void WeaponComponent::LevelUp(int preLv)
{
	switch (preLv)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	// Lv5->Lv6は(今の所)存在しないので終了
	}
}

void WeaponComponent::Reset()
{
}
