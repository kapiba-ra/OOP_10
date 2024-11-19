#include "Weapon.h"

Weapon::Weapon(Game* game)
	: Actor(game)
{
}

Weapon::~Weapon()
{
}

void Weapon::Reset()
{
	SetState(EDead);
}
