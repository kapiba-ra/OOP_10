#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

struct Skill
{
	std::string name;
	int maxLv = 5;
	int curLv = 1;
};

class SkillSystem
{
public:
	SkillSystem(class Game* game);
	~SkillSystem();

	void Reset();

	void AddSkill(const std::string& name, int max = 5);
	//void RemoveSkill(const std::string& name);

	std::vector<Skill*> GetRandomSkills();

private:
	class Game* mGame;

	std::vector<Skill*> mSkills;
	//std::unordered_map<std::string, std::function<void()>> mSkillMap;
};
