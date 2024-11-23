#include "Weapon.h"

Weapon::Weapon(Game* game)
	: Actor(game)
	, mCurLevel(1)
	, mMaxLevel(5)
	, mUniqueInterval(1.0f)
	, mUniqueNum(1)
	, mUniqueSize(1.0f)
	, mUniqueSpeed(1000.0f)
{
}

Weapon::~Weapon()
{
}

void Weapon::Reset()
{
	SetState(EDead);
}

void Weapon::OnLevelUp()
{
	mCurLevel += 1;
}
