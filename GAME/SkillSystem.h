#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Skill.h"

/// <summary>
/// Playerのスキルを管理する.
/// 尚、スキルとは武器とPlayerのステータス(hpや移動速度)の二つを指す。
/// スキル追加するときはとりあえずInitialize()に記述する(必ずしもここである必要はないが)
/// </summary>
class SkillSystem
{
public:
	SkillSystem(class Game* game);
	~SkillSystem();

	void Initialize();
	void Reset();	// 真っ先にResetされることを期待する

	// 武器スキルの設定,functionに注意,初期武器にする場合は第三引数をtrueにする
	void AddWeaponSkill(const std::string& name, std::function<void(class PlayerActor*)> onAcquireSkill);
	// Perkスキルの設定,functionに注意
	void AddPerkSkill(const std::string& name, std::function<void(class PlayerActor*)> levelUpEffect);
	// 初期武器設定用,Playerクラスが呼び出すことを期待
	void SetInitialWeapon(const std::string& name, class PlayerActor* player);

	void OnLevelUp(class Skill* skill);

	//void RemoveSkill(const std::string& name);

	std::vector<Skill*> GetRandomSkills();
	const std::vector<Skill*>& GetAcquiredSkills() const { return mAcquiredSkills; }
	
	//Skill* GetSkill(std::string name) const;

private:
	class Game* mGame;
	std::vector<Skill*> mSkills;
	std::vector<Skill*> mAcquiredSkills;
};
