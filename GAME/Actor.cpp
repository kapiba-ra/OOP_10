#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>


Actor::Actor(Game* game)
	: mState(EActive)
	, mPosition(Vector3::Zero)
	, mRotation(Quaternion::Identity)
	, mScale(1.0f)
	, mGame(game)
	, mRecomputeWorldTransform(true)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);
	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == EActive)
	{
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);

		ComputeWorldTransform();
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{
}

void Actor::ComputeWorldTransform()
{
	if (mRecomputeWorldTransform)
	{
		mRecomputeWorldTransform = false;
		// Order ... Scale, Rotate, Translate
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);

		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

//void Actor::ProcessInput(const uint8_t* keyState)
//{
//	if (mState == EActive)
//	{
//		for (auto comp : mComponents)
//		{
//			comp->ProcessInput(keyState);
//		}
//		ActorInput(keyState);
//	}
//}

void Actor::ProcessInput(const InputState& state)
{
	if (mState == EActive)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(state);
		}
		ActorInput(state);
	}
}

//void Actor::ActorInput(const uint8_t* keyState)
//{
//}

void Actor::ActorInput(const InputState& state)
{
}

void Actor::RotateToNewForward(const Vector3& forward)
{
	// Œ³‚Ì•ûŒü‚ÆV‚µ‚¢•ûŒü‚Ì·‚ð‹‚ß‚é
	float dot = Vector3::Dot(Vector3::UnitX, forward);
	float angle = Math::Acos(dot);
	// •ûŒü‚ªX
	if (dot > 0.9999f)
	{
		SetRotation(Quaternion::Identity);
	}
	// •ûŒü‚ª-X
	else if (dot < -0.9999f)
	{
		SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	}
	else
	{
		// ƒNƒƒXÏ‚Å“¾‚½Ž²Žü‚è‚É‰ñ“]
		Vector3 axis = Vector3::Cross(Vector3::UnitX, forward);
		axis.Normalize();
		SetRotation(Quaternion(axis, angle));
	}
}

void Actor::AddComponent(Component* component)
{
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); iter++)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}
	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}
