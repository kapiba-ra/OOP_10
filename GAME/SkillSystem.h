#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Skill.h"

/// <summary>
/// Playerのスキルを管理する.
/// 尚、スキルとは武器とPlayerのステータス(hpや移動速度)の二つを指す。
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
