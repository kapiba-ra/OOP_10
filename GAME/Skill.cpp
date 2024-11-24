#include "Skill.h"
#include "PlayerActor.h"

Skill::Skill(const std::string& name, std::function<void(PlayerActor*, int)> effect)
	: mName(name)
	, mCurLv(0)
	, mMaxLv(5)
	, mEffect(effect)
{
}

Skill::~Skill()
{
}

void Skill::OnLevelUp(PlayerActor* player)
{
	mCurLv += 1;
	mEffect(player, mCurLv);
}
