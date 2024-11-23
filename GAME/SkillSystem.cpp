#include "SkillSystem.h"
#include "Game.h"
#include <random>

#include "PlayerActor.h"

// ‰¼‚É
#include "HpComponent.h"
#include "ShotComponent.h"

SkillSystem::SkillSystem(Game* game)
	: mGame(game)
{
	AddSkill("MaxHp", [](PlayerActor* player, int curLv) {
		player->GetHpComp()->AddMaxHp(20.0f);
	});
	AddSkill("PlayerSpeed", [](PlayerActor* player, int curLv) {
		player->GetParams().maxForwardSpeed += 50.0f;
	});
	AddSkill("ShotSize", [](PlayerActor* player, int curLv) {
		float scale = player->GetShotComp()->GetBallScale();
		player->GetShotComp()->SetBallScale(scale + 0.2f);
	});
	AddSkill("ShotNum", [](PlayerActor* player, int curLv) {
		int shotNum = player->GetShotComp()->GetShotNum();
		player->GetShotComp()->SetShotNum(shotNum + 1);
	});
	AddSkill("ShotInterval", [](PlayerActor* player, int curLv) {
		float interval = player->GetShotComp()->GetShotInterval();
		interval *= 0.9f;
		player->GetShotComp()->SetShotInterval(interval);
	});
	AddSkill("ShotSpeed", [](PlayerActor* player, int curLv) {
		player->GetShotComp()->IncShotSpeed(100.0f);
	});

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
		//skill->curLv = 1;
		skill->SetCurLv(1);
	}
}

void SkillSystem::AddSkill(const std::string& name, std::function<void(class PlayerActor*, int)> effect)
{
	Skill* skill = new Skill(name, effect);
	mSkills.push_back(skill);
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
		//if (skill->curLv < skill->maxLv) 
		if (!skill->IsLevelMax()) 
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

