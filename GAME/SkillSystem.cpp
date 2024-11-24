#include "SkillSystem.h"
#include "Game.h"
#include <random>

#include "PlayerActor.h"

// 仮に
#include "HpComponent.h"
#include "ShotComponent.h"
#include "SwordComponent.h"

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

	// WeaponCommonの処理
	//AddSkill("WeaponSize", [](PlayerActor* player, int curLv) {
	//	player->GetParams().WeaponSizeFactor *= 1.2f;
	//});
	//AddSkill("WeaponInterval", [](PlayerActor* player, int curLv) {
	//	player->GetParams().WeaponIntervalFactor *= 0.9f;
	//});
	//AddSkill("WeaponSpeed", [](PlayerActor* player, int curLv) {
	//	player->GetParams().WeaponSpeedFactor *= 1.1f;
	//});
	// WeaponCommonの処理


	// ボール	BallWeapon
	// カタナ	SwordWeapon
	AddSkill("Sword", [this](PlayerActor* player, int curLv) {
		if (curLv == 1)
		{
			SwordComponent* sc = new SwordComponent(player);
			player->AddWeapon(sc);
		}
		else
		{
			// player->LvUpWeapon(name, curLv);
		}
	});

	// 矢		ArrowWeapon
	// 犬		DogWeapon
	// 爆弾		BombWeapon
	//AddSkill("Ball", [](PlayerActor* player, int curLv) {
	//	// 一連の処理はShotComponentへ移す方がいいだろう
	//	switch (curLv)
	//	{
	//	case 1:	// ここは発動しないだろう
	//		ShotComponent* sc = new ShotComponent(player);
	//		player->AddWeapon(sc);
	//		// plyaer->SetFactors(sc);のような関数
	//	case 2:
	//		int shotNum = player->GetShotComp()->GetShotNum();
	//		player->GetShotComp()->SetShotNum(shotNum + 1);
	//		float interval = player->GetShotComp()->GetShotInterval();
	//		interval *= 0.9f;
	//		player->GetShotComp()->SetShotInterval(interval);
	//	case 3:
	//		int shotNum = player->GetShotComp()->GetShotNum();
	//		player->GetShotComp()->SetShotNum(shotNum + 1);
	//	case 4:
	//		int shotNum = player->GetShotComp()->GetShotNum();
	//		player->GetShotComp()->SetShotNum(shotNum + 1);
	//	case 5:
	//		int shotNum = player->GetShotComp()->GetShotNum();
	//		player->GetShotComp()->SetShotNum(shotNum + 1);
	//	}
	//});
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

//Skill* SkillSystem::GetSkill(std::string name) const
//{
//	for (const auto& skill : mSkills)
//	{
//		if (skill->GetName() == name)
//		{
//			return skill;
//			break;
//		}
//	}
//	return nullptr;
//}

