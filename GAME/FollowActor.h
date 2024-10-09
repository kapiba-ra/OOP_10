#pragma once
#include "Actor.h"

class FollowActor :
	public Actor
{
public:
	FollowActor(class Game* game);

	//void ActorInput(const uint8_t* keys) override;
	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	
	enum State
	{
		EOnFloor,
		EJumping,
		EFalling
	};
	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

	float GetJumpSpeed() const { return mJumpSpeed; }
	void SetJumpSpeed(float speed) { mJumpSpeed = speed; }

private:
	State mState;

	void Jump(float deltaTime);
	float mJumpSpeed;
	void AutoShoot(float deltaTime);
	float mShotInterval;
	float mLastShot;

	int mHP;

	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
};

