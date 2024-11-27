#pragma once
#include <string>
#include <functional>

///// <summary>
///// Player�����e�X�L���Ɋւ���N���X
///// Skill��enum�ϐ�Type���������A���킩�p�����[�^�֘A���ŏ����𕪂�����@�������Ă݂���
///// </summary>
//class Skill
//{
//public:
//	enum class Type
//	{
//		EWeapon,	// ����n
//		EPerk		// Player��Parameter�֘A(Maxhp,moveSpeed,etc...)
//	};
//
//	Skill(const std::string& name, Type type, std::function<void(class PlayerActor* player)> onGetSkill);
//	~Skill();
//
//	void OnLevelUp(class PlayerActor* player);
//	
//	std::string GetName() const { return mName; }
//	void SetCurLv(int lv) { mCurLv = lv; }
//
//	bool IsLevelMax() const { return mCurLv == mMaxLv; }
//
//private:
//	enum class Type mType;
//	std::string mName;	// �X�L����
//	std::function<void(class PlayerActor*, int)> mEffect;	// LevelUp���̌���.int��mCurLv��z��
//	
//	// Skill�l����(Lv0->1)�̏����͊O�����猈�߂�,Skill���ɈقȂ�Component�𓮓I�ɃA�^�b�`���邽��
//	// Skill�����g���΂����瑤�����ŏ����ł���������,�X�L������Switch����̂�
//	// ���̃N���X�̒��g������������K�v������,�璷�Ȃ̂ŋp��
//	/* �ȏ�̔Y�݂̎Y�� */ 
//	std::function<void(class PlayerActor*)> mOnGetSkill;
//	
//	int mMaxLv;
//	int mCurLv;
//};

/// <summary>
/// Skill�N���X
/// Weapon��Perk�̓�ɔh������,LevelUp���̏������e�����\�قȂ�̂ŃN���X��������
/// </summary>
class Skill
{
public:
	// �X�L���͂��̓�ɕ��ނ����
	enum class Type
	{
		EWeapon,
		EPerk
	};

	Skill(const std::string& name, Type type);
	virtual ~Skill();

	virtual void LevelUp(class PlayerActor* player) {};

	std::string GetName() const { return mName; }
	void SetCurLv(int lv) { mCurLv = lv; }
	int GetCurLv() const { return mCurLv; }
	class Texture* GetIconTex() const { return mIcon; }
	void SetIconTex(class Texture* tex) { mIcon = tex; }
	Type GetType() const { return mType; }

	bool IsLevelMax() const { return mCurLv == mMaxLv; }

protected:
	std::string mName;		// �X�L����
	class Texture* mIcon;	// �X�L���̃A�C�R���摜
	Type mType;				// �X�L���̕���(����orPerk)
	int mCurLv;				// 0���������
	int mMaxLv;				// 5���f�t�H���g��Max
};

/// <summary>
/// Player��������Ɋ֘A����X�L��
/// </summary>
class WeaponSkill
	: public Skill
{
public:
	WeaponSkill(const std::string& name,
		std::function<void(class PlayerActor* player)> onAcquireSkill,
		Type type);

	void LevelUp(class PlayerActor* player) override;
	
private:
	std::function<void(class PlayerActor*)> mOnAcquireSkill;
};

/// <summary>
/// Player���g������Parameter��������X�L����Perk�ƌĂ�ł���(�ړ����x��MaxHp�Ȃ�)
/// </summary>
class PerkSkill
	: public Skill
{
public:
	PerkSkill(const std::string& name,
		std::function<void(class PlayerActor* player)> effect,
		Type type);

	void LevelUp(class PlayerActor* player) override;

private:
	// Perk��LevelUp���̏�����\�ߊO�����猈�߂Ă��炤�K�v������
	std::function<void(class PlayerActor*)> mEffect;
};

// Weapon��Perk��function���󂯎��̂�,mEffect��mOnAcquireSkill�͊��N���X��
// �܂Ƃ߂邱�Ƃ͂ł���B�������������Ⴄ�̂ł�߂Ƃ���