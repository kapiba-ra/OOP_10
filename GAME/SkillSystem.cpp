#include "SkillSystem.h"
#include <random>
#include <numeric>
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "HUD.h"

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

	Initialize();

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

void SkillSystem::Initialize()
{
	// 新たなるAddSkill関連
	// AddWeaponSkillで設定する関数はスキル獲得時の処理
	// AddPerkSkillで設定する関数はレベルアップ時の処理
	
	// WeaponはPlayerの方に一旦おいてみる
	// Weapon
	AddWeaponSkill("Sword", [this](PlayerActor* player) {
		SwordComponent* sc = new SwordComponent(player);
		player->AddWeapon("Sword", sc);
	});
	AddWeaponSkill("Gun", [](PlayerActor* player) {
		ShotComponent* sc = new ShotComponent(player);
		player->AddWeapon("Gun", sc);
	});


	// 以下Perk
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
}

void SkillSystem::Reset()
{
	for (auto skill : mSkills)
	{
		skill->SetCurLv(0);
	}
	mAcquiredSkills.clear();
}

void SkillSystem::AddWeaponSkill(const std::string& name, std::function<void(PlayerActor*)> onAcquireSkill)
{
	WeaponSkill* skill = new WeaponSkill(name, onAcquireSkill, Skill::Type::EWeapon);
	// Iconを設定
	Texture* tex = mGame->GetRenderer()->GetTexture("Assets/Icon/" + name + ".png");
	skill->SetIconTex(tex);
	// 配列に追加
	mSkills.push_back(skill);
}

void SkillSystem::AddPerkSkill(const std::string& name, std::function<void(class PlayerActor*)> levelUpEffect)
{
	PerkSkill* skill = new PerkSkill(name, levelUpEffect, Skill::Type::EPerk);
	// Iconを設定
	Texture* tex = mGame->GetRenderer()->GetTexture("Assets/Icon/" + name + ".png");
	skill->SetIconTex(tex);
	// 配列に追加
	mSkills.push_back(skill);
}

void SkillSystem::SetInitialWeapon(const std::string& name, PlayerActor* player)
{
	for (const auto& skill : mSkills)
	{
		if (skill->GetName() == name && skill->GetCurLv() == 0) // 一応Lv0であることをチェック
		{
			mAcquiredSkills.push_back(skill);
			// 現状,Plaeyr初期化中に呼ばれるので,playerをmGame->GetPlayer()とするのはおかしい
			skill->LevelUp(player);
		}
	}
}

void SkillSystem::OnLevelUp(Skill* skill)
{
	if (skill->GetCurLv() == 0)
	{
		mAcquiredSkills.push_back(skill);
	}
	skill->LevelUp(mGame->GetPlayer());
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
	std::vector<int> indices(mSkills.size());
	std::iota(indices.begin(), indices.end(), 0); // 0, 1, 2, ..., mSkills.size() - 1

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(indices.begin(), indices.end(), gen); // インデックスだけをシャッフル

	for (int i : indices)
	{
		if (!mSkills[i]->IsLevelMax())
		{
			ret.push_back(mSkills[i]);
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

