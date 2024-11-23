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

	void AddSkill(const std::string& name, std::function<void(class PlayerActor*, int)> effect);
	//void RemoveSkill(const std::string& name);

	std::vector<Skill*> GetRandomSkills();

private:
	class Game* mGame;
	std::vector<Skill*> mSkills;
};
