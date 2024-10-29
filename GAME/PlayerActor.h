#pragma once
#include "Actor.h"

class PlayerActor :
	public Actor
{
public:
	PlayerActor(class Game* game);

	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	void Reset() override;

	enum PosState
	{
		EOnFloor,
		EJumping,	// fallingと分けてるけど現状は活用してない
		EFalling
	};
	PosState GetPosState() const { return mPosState; }
	void SetPosState(PosState state) { mPosState = state; }

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

	void TakeDamage(float amount);

	void GainExp(float exp);
	void IncShotNum() { mParams.shotNum += 1; }
	void MoveSpeedUp();

	class BoxComponent* GetBox() { return mBoxComp; }

private:
	PosState mPosState;

	void Jump(float deltaTime);
	void AutoShoot(float deltaTime);
	void CheckLevelUp();

	// parameterとしてまとめられそう
	//float mShotInterval;
	//float mJumpSpeed;
	//float mMaxForwardSpeed;
	//float mHP;
	//float mExp;
	//float mExpToLevelUp;
	//int mLevel;
	//int mShotNum;
	//float mLastShot;

	class HUD* mHUD;
	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;

	// parameter related
	// Dataクラスから初期化したいかも
public:
	struct Parameters
	{
		float maxForwardSpeed = 400.0f;
		float jumpSpeed = 0.0f;
		float maxHp = 100.0f;
		float hp = maxHp;
		float exp = 0.0f;
		float expToLevelUp = 1.0f;
		int level = 1;
		// shot系の機能はcomponentにまとめるべきかも(敵が撃つなら)
		int shotNum = 1;
		float lastShot = 0.0f;
		float shotInterval = 1.0f;

		void Reset();
	};
	// 書き換え可能にしている
	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

