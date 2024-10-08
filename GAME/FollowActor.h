#pragma once
#include "Actor.h"

class FollowActor :
	public Actor
{
public:
	FollowActor(class Game* game);

	void ActorInput(const uint8_t* keys) override;
	void UpdateActor(float deltaTime) override;
	
	enum State
	{
		EOnFloor,
		EJumping,
		EFalling
	};

	// 2つのAABBの最小の重なりテストを行う
	void FixCollisions();

private:
	void Jump();
	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
};

