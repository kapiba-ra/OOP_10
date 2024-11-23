#include "Weapon.h"

Weapon::Weapon(Game* game)
	: Actor(game)
	, mCurLevel(1)
	, mMaxLevel(5)
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
