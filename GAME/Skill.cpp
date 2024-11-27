#include "Skill.h"
#include "PlayerActor.h"
#include "Texture.h"
#include "SkillSystem.h"

//Skill::Skill(const std::string& name, std::function<void(PlayerActor*, int)> effect)
//	: mName(name)
//	, mCurLv(0)
//	, mMaxLv(5)
//	, mEffect(effect)
//{
//}
//
//
//Skill::Skill(const std::string& name, Type type, std::function<void(PlayerActor* player)> onGetSkill)
//	: mName(name)
//	, mCurLv(0)	// 初期値が0である事に注意(そういう仕様)
//	, mMaxLv(5)
//	, mType(type)
//	, mOnGetSkill(onGetSkill)
//{
//}
//
//Skill::~Skill()
//{
//}
//
//void Skill::OnLevelUp(PlayerActor* player)
//{
//	//mCurLv += 1;
//	//mEffect(player, mCurLv);
//
//	// スキルのTypeによって処理を分けた
//	if (mType == Type::EWeapon)
//	{
//		if (mCurLv == 0)
//		{
//			mOnGetSkill(player); // std::function です
//		}
//		else
//		{
//			player->LevelUpWeapon(mName, mCurLv);
//		}
//	}
//	else if (mType == Type::EPerk)
//	{
//		if (mCurLv == 0)
//		{
//			player->LevelUpPerk(mName, mCurLv);
//		}
//		else
//		{
//			player->LevelUpPerk(mName, mCurLv);
//		}
//	}
//	mCurLv++;
//
//}

Skill::Skill(const std::string& name, Type type)
	: mName(name)
	, mIcon(nullptr)
	, mType(type)
	, mCurLv(0)	// 初期値が0である事に注意(そういう仕様)
	, mMaxLv(5)
{
	// こっちから見えないのが少々分かりづらいが、mIconは
	// SkillSystemのAddWeapon()内でしっかり設定されている
}

Skill::~Skill()
{
}

WeaponSkill::WeaponSkill(const std::string& name,
	std::function<void(class PlayerActor* player)> onAcquireSkill,
	Type type)
	: Skill(name, type)
	, mOnAcquireSkill(onAcquireSkill)
{
}

void WeaponSkill::LevelUp(PlayerActor* player)
{
	if (mCurLv == 0 && player != nullptr)
	{
		// PlayerのAddWeaponを呼び出す必要がある
		// その際にWeaponComponentのポインタを渡す必要がある
		mOnAcquireSkill(player);
	}
	else
	{
		player->LevelUpWeapon(mName, mCurLv);
	}

	// ここで次のレベルになる
	mCurLv += 1;
}

PerkSkill::PerkSkill(const std::string& name,
	std::function<void(class PlayerActor* player)> effect,
	Type type)
	: Skill(name, type)
	, mEffect(effect)
{
}

void PerkSkill::LevelUp(PlayerActor* player)
{
	// とりあえずこんだけ,スキル獲得時にIconをどこかに追加するような処理が入るかも
	// PerkはWeaponと違ってスキル獲得時にPlayerに特定のComponentを渡すわけではないので、
	// Weaponが持つOnAcquireSkillのようなfunctionはいらない...はず
	mEffect(player);

	// ここで次のレベルになる
	mCurLv += 1;
}
