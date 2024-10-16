#pragma once
#include "Actor.h"

class PlayerActor :
	public Actor
{
public:
	PlayerActor(class Game* game);

	//void ActorInput(const uint8_t* keys) override;
	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	
	void Reset() override;

	enum PosState
	{
		EOnFloor,
		EJumping,
		EFalling
	};
	PosState GetPosState() const { return mPosState; }
	void SetPosState(PosState state) { mPosState = state; }

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

	void TakeDamage(float amount);

	float GetJumpSpeed() const { return mJumpSpeed; }
	void SetJumpSpeed(float speed) { mJumpSpeed = speed; }
	float GetHP() const { return mHP; }
	void SetHP(float hp) { mHP = hp; }
	class BoxComponent* GetBox() { return mBoxComp; }


private:
	PosState mPosState;

	void Jump(float deltaTime);
	float mJumpSpeed;
	void AutoShoot(float deltaTime);
	float mShotInterval;
	float mLastShot;

	float mHP;
	class HUD* mHUD;

	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
};

