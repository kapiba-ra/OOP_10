#include "SkillSystem.h"
#include <random>

SkillSystem::SkillSystem(Game* game)
	: mGame(game)
{
	// —Í‹Z Power play
	AddSkill("MaxHp");
	AddSkill("PlayerSpeed");
	AddSkill("ShotSize");
	AddSkill("ShotNum");
	AddSkill("ShotInterval");
	AddSkill("ShotSpeed");
}

SkillSystem::~SkillSystem()
{
	for (auto skill : mSkills)
	{
		delete skill;
	}
	mSkills.clear();
}

void SkillSystem::Reset()
{
	for (auto skill : mSkills)
	{
		skill->curLv = 1;
	}
}

void SkillSystem::AddSkill(const std::string& name, int max)
{
	Skill* skill = new Skill(); // delete•K—v
	skill->name = name;
	skill->maxLv = max;
	mSkills.emplace_back(skill);
}

//void SkillSystem::RemoveSkill(const std::string& name)
//{
//	for (auto iter = mSkills.begin(); iter != mSkills.end(); )
//	{
//		if ((*iter)->name == name)
//		{
//			delete *iter;
//			mSkills.erase(iter);
//		}
//		else
//		{
//			++iter;
//		}
//	}
//}

std::vector<Skill*> SkillSystem::GetRandomSkills()
{
	std::vector<Skill*> ret;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(mSkills.begin(), mSkills.end(), gen);

	for (const auto& skill : mSkills)
	{
		if (skill->curLv < skill->maxLv)
		{
			ret.push_back(skill);
			if (ret.size() == 3)
			{
				break;
			}
		}
	}

	return ret;
}

