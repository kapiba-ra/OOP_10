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
//	, mCurLv(0)	// �����l��0�ł��鎖�ɒ���(���������d�l)
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
//	// �X�L����Type�ɂ���ď����𕪂���
//	if (mType == Type::EWeapon)
//	{
//		if (mCurLv == 0)
//		{
//			mOnGetSkill(player); // std::function �ł�
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
	, mCurLv(0)	// �����l��0�ł��鎖�ɒ���(���������d�l)
	, mMaxLv(5)
{
	// ���������猩���Ȃ��̂����X������Â炢���AmIcon��
	// SkillSystem��AddWeapon()���ł�������ݒ肳��Ă���
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
		// Player��AddWeapon���Ăяo���K�v������
		// ���̍ۂ�WeaponComponent�̃|�C���^��n���K�v������
		mOnAcquireSkill(player);
	}
	else
	{
		player->LevelUpWeapon(mName, mCurLv);
	}

	// �����Ŏ��̃��x���ɂȂ�
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
	// �Ƃ肠�������񂾂�,�X�L���l������Icon���ǂ����ɒǉ�����悤�ȏ��������邩��
	// Perk��Weapon�ƈ���ăX�L���l������Player�ɓ����Component��n���킯�ł͂Ȃ��̂ŁA
	// Weapon������OnAcquireSkill�̂悤��function�͂���Ȃ�...�͂�
	mEffect(player);

	// �����Ŏ��̃��x���ɂȂ�
	mCurLv += 1;
}
