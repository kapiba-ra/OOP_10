#include "JumpComponent.h"
#include "Math.h"
#include "Actor.h"

JumpComponent::JumpComponent(Actor* owner, int updateOrder)
	: Component(owner)
	, mJumpSpeed(0.0f)
	, mIsJumping(false)
	, mGravity(1000.0f)
{
}

void JumpComponent::Update(float deltaTime)
{
	if (mIsJumping)
	{
		Vector3 pos = mOwner->GetPosition();
		pos += Vector3::UnitZ * mJumpSpeed * deltaTime;
		mJumpSpeed -= mGravity * deltaTime;

		mOwner->SetPosition(pos);
	}
}

void JumpComponent::Liftoff(float speed)
{
	mJumpSpeed = speed;
	mIsJumping = true;
}

void JumpComponent::Land()
{
	mIsJumping = false;
	mJumpSpeed = 0.0f;
}
