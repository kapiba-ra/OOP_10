#pragma once
#include "Actor.h"
#include <unordered_map>
#include <string>

class PlayerActor :
	public Actor
{
public:
	PlayerActor(class Game* game);
	~PlayerActor();

	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	void Reset() override;

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

	void GainExp(float exp);

	class BoxComponent* GetBox() { return mBoxComp; }
	class HpComponent* GetHpComp() { return mHpComp; }
	// 後で消す
	void AddWeapon(std::string name, class WeaponComponent* weapon);
	// nameはいずれ参照になる(enumでskillのタイプを分ける)
	void LevelUpWeapon(std::string name, int lv);
	// 基底クラスがメモリは解放してくれるのでとりあえず要らない
	//void RemoveWeapon(WeaponComponent* weapon);
	float GetForwardSpeed();
	
	// 全てのWeaponFactorsを今持つすべての武器に適用する
	void ApplyWeaponFactors();
	// 指定の名前のボーンのワールド空間位置を計算する
	class Vector3 GetBoneWorldPosition(std::string boneName);

private:
	void CheckLevelUp();

	class HUD* mHUD;
	// ステータス,パラメータに関連するもの
	class MoveComponent* mMoveComp;
	class JumpComponent* mJumpComp;
	class HpComponent* mHpComp;
	// essential
	class FollowCamera* mCameraComp;
	class SkeletalMeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	class AudioComponent* mAudioComp;
	// weapon
	std::unordered_map<std::string, class WeaponComponent*> mWeapons;

	bool mMoving;

public:
	// parameter related, Perkとも関連する,というか呼び名Perkに統一しようかな
	struct Parameters
	{
		float maxForwardSpeed = 400.0f;
		float maxJumpSpeed = 700.0f;
		float WeaponSizeFactor = 1.0f;
		float WeaponIntervalFactor= 1.0f;
		float WeaponSpeedFactor = 1.0f;

		float expToLevelUp = 1.0f;
		float exp = 0.0f;
		int level = 1;

		void Reset();
	};
	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

