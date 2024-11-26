#include "SkillSystem.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include <random>

#include "PlayerActor.h"

// 仮に
#include "HpComponent.h"
#include "ShotComponent.h"
#include "SwordComponent.h"

SkillSystem::SkillSystem(Game* game)
	: mGame(game)
{
	//AddSkill("MaxHp", [](PlayerActor* player, int curLv) {
	//	player->GetHpComp()->AddMaxHp(20.0f);
	//});
	//AddSkill("PlayerSpeed", [](PlayerActor* player, int curLv) {
	//	player->GetParams().maxForwardSpeed += 50.0f;
	//});
	//AddSkill("ShotSize", [](PlayerActor* player, int curLv) {
	//	float scale = player->GetShotComp()->GetBallScale();
	//	player->GetShotComp()->SetBallScale(scale + 0.2f);
	//});
	//AddSkill("ShotNum", [](PlayerActor* player, int curLv) {
	//	int shotNum = player->GetShotComp()->GetShotNum();
	//	player->GetShotComp()->SetShotNum(shotNum + 1);
	//});
	//AddSkill("ShotInterval", [](PlayerActor* player, int curLv) {
	//	float interval = player->GetShotComp()->GetShotInterval();
	//	interval *= 0.9f;
	//	player->GetShotComp()->SetShotInterval(interval);
	//});
	//AddSkill("ShotSpeed", [](PlayerActor* player, int curLv) {
	//	player->GetShotComp()->IncShotSpeed(100.0f);
	//});
	//
	// WeaponCommonの処理
	//AddSkill("ShotSize", Skill::Type::EPerk, [](PlayerActor* player) {
	//	player->AddPerk("ShotSize");
	//});
	//AddSkill("ShotInterval", Skill::Type::EPerk, [](PlayerActor* player) {
	//	player->AddPerk("ShotInterval");
	//});
	//AddSkill("ShotSpeed", Skill::Type::EPerk, [](PlayerActor* player) {
	//	player->AddPerk("ShotSpeed");
	//});
	// WeaponCommonの処理


	// ボール	BallWeapon
	// カタナ	SwordWeapon
	//AddSkill("Sword", [this](PlayerActor* player, int curLv) {
	//	if (curLv == 1)
	//	{
	//		SwordComponent* sc = new SwordComponent(player);
	//		player->AddWeapon("Sword", sc);
	//	}
	//	else
	//	{
	//		player->LevelUpWeapon("Sword", curLv);
	//	}
	//});

	///* 新世代のAddSkill */
	//// weapon
	//AddSkill("Sword", Skill::Type::EWeapon, [](PlayerActor* player) {
	//	SwordComponent* sc = new SwordComponent(player);
	//	player->AddWeapon("Sword", sc);
	//});
	////AddSkill("Gun", Skill::Type::EWeapon, [](PlayerActor* player) {
	//	// なにも設定しないが、これは仮の姿(初期武器固定の姿)
	//	//
	//	//ShotComponent* sc = new ShotComponent(player);
	//	//player->AddWeapon("Gun", sc);
	////});
	//
	//// parameter
	//AddSkill("MaxHp", Skill::Type::EPerk, [](PlayerActor* player) {
	//	player->AddPerk("MaxHp");
	//});
	//AddSkill("PlayerSpeed", Skill::Type::EPerk, [](PlayerActor* player) {
	//	player->AddPerk("PlayerSpeed");
	//});

	// 新たなるAddSkill関連
	// AddWeaponSkillで設定する関数はスキル獲得時の処理
	// AddPerkSkillで設定する関数はレベルアップ時の処理
	AddWeaponSkill("Sword", [](PlayerActor* player) {
		SwordComponent* sc = new SwordComponent(player);
		player->AddWeapon("Sword", sc);
	});
	AddPerkSkill("MaxHp", [](PlayerActor* player) {
		float add = 20.0f;
		player->GetHpComp()->AddMaxHp(add);
	});
	AddPerkSkill("PlayerSpeed", [](PlayerActor* player) {
		float add = 100.0f;
		player->GetParams().maxForwardSpeed += add;
	});
	AddPerkSkill("ShotSize", [](PlayerActor* player) {
		float multiplier = 1.2f;
		player->GetParams().WeaponSizeFactor *= multiplier;
		player->ApplyWeaponFactors();
	});
	AddPerkSkill("ShotInterval", [](PlayerActor* player) {
		float multiplier = 0.9f;
		player->GetParams().WeaponIntervalFactor *= multiplier;
		player->ApplyWeaponFactors();
	});
	AddPerkSkill("ShotSpeed", [](PlayerActor* player) {
		float multiplier = 1.2f;
		player->GetParams().WeaponSpeedFactor *= multiplier;
		player->ApplyWeaponFactors();
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
		skill->SetCurLv(0);
	}
}

//void SkillSystem::AddSkill(const std::string& name, std::function<void(class PlayerActor*, int)> effect)
//{
//	Skill* skill = new Skill(name, effect);
//	mSkills.push_back(skill);
//}

//void SkillSystem::AddSkill(const std::string& name, Skill::Type type, std::function<void(PlayerActor*)> onGetSkill)
//{
//	Skill* skill = new Skill(name, type, onGetSkill);
//	mSkills.push_back(skill);
//}

void SkillSystem::AddWeaponSkill(const std::string& name, std::function<void(PlayerActor*)> onAcquireSkill)
{
	WeaponSkill* skill = new WeaponSkill(name, onAcquireSkill);
	// Iconを設定
	Texture* tex = mGame->GetRenderer()->GetTexture("Assets/Icon/" + name + ".png");
	skill->SetIconTex(tex);
	// 配列に追加
	mSkills.push_back(skill);
}

void SkillSystem::AddPerkSkill(const std::string& name, std::function<void(class PlayerActor*)> levelUpEffect)
{
	PerkSkill* skill = new PerkSkill(name, levelUpEffect);
	// Iconを設定
	Texture* tex = mGame->GetRenderer()->GetTexture("Assets/Icon/" + name + ".png");
	skill->SetIconTex(tex);
	// 配列に追加
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

