#include "WeaponActor.h"

#include "DamageComponent.h"

WeaponActor::WeaponActor(Game* game)
	: Actor(game, Type::Eweapon)
	, mLifeSpan(2.0f)
{
	mDamageComp = new DamageComponent(this);
}

WeaponActor::~WeaponActor()
{
}

void WeaponActor::Reset()
{
	SetState(EDead);
}

void WeaponActor::HitTarget(Actor* target)
{
	mDamageComp->Damage(target);
}
