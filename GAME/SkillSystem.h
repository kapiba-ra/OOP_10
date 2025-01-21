#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Skill.h"

/// <summary>
/// Player�̃X�L�����Ǘ�����.
/// ���A�X�L���Ƃ͕����Player�̃X�e�[�^�X(hp��ړ����x)�̓���w���B
/// �X�L���ǉ�����Ƃ��͂Ƃ肠����Initialize()�ɋL�q����(�K�����������ł���K�v�͂Ȃ���)
/// </summary>
class SkillSystem
{
public:
	SkillSystem(class Game* game);
	~SkillSystem();

	void Initialize();
	void Reset();	// �^�����Reset����邱�Ƃ����҂���

	// ����X�L���̐ݒ�,function�̓X�L���l�����̏������L�q,��������ɂ���ꍇ�͑�O������true�ɂ���
	void AddWeaponSkill(const std::string& name, std::function<void(class PlayerActor*)> onAcquireSkill);
	// Perk�X�L���̐ݒ�,function�̓��x���A�b�v���̌��ʂ�ݒ�
	void AddPerkSkill(const std::string& name, std::function<void(class PlayerActor*)> levelUpEffect);
	// ��������ݒ�p,Player�N���X���Ăяo�����Ƃ�����
	void SetInitialWeapon(const std::string& name, class PlayerActor* player);

	void OnLevelUp(class Skill* skill);

	//void RemoveSkill(const std::string& name);

	std::vector<Skill*> GetRandomSkills();
	const std::vector<Skill*>& GetAcquiredSkills() const { return mAcquiredSkills; }
	class Texture* GetRecoverIcon() { return mRecoverIcon; }
	//Skill* GetSkill(std::string name) const;

private:
	class Game* mGame;
	std::vector<Skill*> mSkills;
	std::vector<Skill*> mAcquiredSkills;
	class Texture* mRecoverIcon; // �S�ẴX�L�����ő僌�x���ɂȂ����Ƃ��Ɏg���A�C�R��
};
