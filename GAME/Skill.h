#pragma once
#include <string>
#include <functional>

///// <summary>
///// Playerが持つ各スキルに関するクラス
///// Skillにenum変数Typeを持たせ、武器かパラメータ関連かで処理を分ける方法を試してみたい
///// </summary>
//class Skill
//{
//public:
//	enum class Type
//	{
//		EWeapon,	// 武器系
//		EPerk		// PlayerのParameter関連(Maxhp,moveSpeed,etc...)
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
//	std::string mName;	// スキル名
//	std::function<void(class PlayerActor*, int)> mEffect;	// LevelUp時の効果.intはmCurLvを想定
//	
//	// Skill獲得時(Lv0->1)の処理は外部から決める,Skill毎に異なるComponentを動的にアタッチするため
//	// Skill名を使えばこちら側だけで処理できそうだが,スキル名でSwitchするのは
//	// このクラスの中身を書き換える必要があり,冗長なので却下
//	/* 以上の悩みの産物 */ 
//	std::function<void(class PlayerActor*)> mOnGetSkill;
//	
//	int mMaxLv;
//	int mCurLv;
//};

/// <summary>
/// Skillクラス
/// WeaponとPerkの二つに派生する,LevelUp時の処理内容が結構異なるのでクラス分けした
/// </summary>
class Skill
{
public:
	// スキルはこの二つに分類される
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
	std::string mName;		// スキル名
	class Texture* mIcon;	// スキルのアイコン画像
	Type mType;				// スキルの分類(武器orPerk)
	int mCurLv;				// 0が初期状態
	int mMaxLv;				// 5がデフォルトのMax
};

/// <summary>
/// Playerが持つ武器に関連するスキル
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
/// Player自身が持つParameterをいじるスキルをPerkと呼んでいる(移動速度やMaxHpなど)
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
	// PerkはLevelUp時の処理を予め外部から決めてもらう必要がある
	std::function<void(class PlayerActor*)> mEffect;
};

// WeaponもPerkもfunctionを受け取るので,mEffectとmOnAcquireSkillは基底クラスに
// まとめることはできる。しかし役割が違うのでやめといた