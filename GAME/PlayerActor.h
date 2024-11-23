#pragma once
#include "Actor.h"
#include <unordered_map>
#include <string>

class PlayerActor :
	public Actor
{
public:
	PlayerActor(class Game* game);

	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	void Reset() override;

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

	//void TakeDamage(float amount);

	void GainExp(float exp);
	//void GainHeart(float recover);

	class BoxComponent* GetBox() { return mBoxComp; }
	class HpComponent* GetHpComp() { return mHpComp; }
	// 後で消す
	class ShotComponent* GetShotComp() { return mShotComp; }

private:
	void CheckLevelUp();

	class HUD* mHUD;
	// ステータス,パラメータに関連するもの
	class MoveComponent* mMoveComp;
	class JumpComponent* mJumpComp;
	class HpComponent* mHpComp;

	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	class ShotComponent* mShotComp;
	class AudioComponent* mAudioComp;

public:
	// parameter related
	struct Parameters
	{
		float maxForwardSpeed = 400.0f;
		float maxJumpSpeed = 500.0f;
		float expToLevelUp = 1.0f;
		float exp = 0.0f;
		int level = 1;

		void Reset();
	};
	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

