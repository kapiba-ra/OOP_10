#pragma once
#include <string>
#include <functional>

/// <summary>
/// Playerが持つ各スキルに関するクラス
/// </summary>
class Skill
{
public:
	Skill(const std::string& name, std::function<void(class PlayerActor*, int)> effect);
	~Skill();

	void OnLevelUp(class PlayerActor* player);

	bool IsLevelMax() const { return mCurLv == mMaxLv; }

	std::string GetName() const { return mName; }
	int GetCurLv() const { return mCurLv; }
	int GetMaxLv() const { return mMaxLv; }
	void SetCurLv(int lv) { mCurLv = lv; }
	void SetMaxLv(int lv) { mMaxLv = lv; }

private:
	std::string mName;	// スキル名
	std::function<void(class PlayerActor*, int)> mEffect;	// LevelUp時の効果.intはmCurLvを想定
	int mMaxLv;
	int mCurLv;
};

