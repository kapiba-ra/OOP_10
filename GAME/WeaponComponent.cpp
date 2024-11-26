#include "WeaponComponent.h"
#include "PlayerActor.h"
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
	//if (mOwner->GetType() == Actor::Eplayer)
	//{
	//	static_cast<PlayerActor*>(mOwner)->RemoveWeapon(this);
	//}
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
	// Lv5->Lv6‚Í(¡‚ÌŠ)‘¶İ‚µ‚È‚¢‚Ì‚ÅI—¹
	}
}
