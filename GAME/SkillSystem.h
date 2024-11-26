#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Skill.h"

/// <summary>
/// Player�̃X�L�����Ǘ�����.
/// ���A�X�L���Ƃ͕����Player�̃X�e�[�^�X(hp��ړ����x)�̓���w���B
/// </summary>
class SkillSystem
{
public:
	SkillSystem(class Game* game);
	~SkillSystem();

	void Reset();

	//void AddSkill(const std::string& name, std::function<void(class PlayerActor*, int)> effect);
	//void AddSkill(const std::string& name, Skill::Type type, std::function<void(class PlayerActor*)> onGetSkill);
	void AddWeaponSkill(const std::string& name, std::function<void(class PlayerActor*)> onAcquireSkill);
	void AddPerkSkill(const std::string& name, std::function<void(class PlayerActor*)> levelUpEffect);
	//void RemoveSkill(const std::string& name);

	std::vector<Skill*> GetRandomSkills();
	//Skill* GetSkill(std::string name) const;

private:
	class Game* mGame;
	std::vector<Skill*> mSkills;
};
