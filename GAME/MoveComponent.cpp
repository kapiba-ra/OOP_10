#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mAngularSpeed(0.0f)
	, mForwardSpeed(0.0f)
	, mStrafeSpeed(0.0f)
	//, mJumpSpeed(0.0f)
{
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rota = mOwner->GetRotation();
		float angle = mAngularSpeed * deltaTime;
		// ZŽ²Žü‚è‚ð‰ñ“]
		// ‘•ª‰ñ“](incremental rotation)
		Quaternion inc(Vector3::UnitZ, angle);
		rota = Quaternion::Concatenate(rota, inc);
		mOwner->SetRotation(rota);
	}

	if (!Math::NearZero(mForwardSpeed) ||
		!Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}
}