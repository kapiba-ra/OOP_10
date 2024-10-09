#pragma once
#include <vector>
#include <cstdint>
#include "Math.h"

class Actor
{
public:
	enum State
	{
		EActive,
		EPaused,
		EDead
	};

	Actor(class Game* game);
	virtual ~Actor();

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);
	void ComputeWorldTransform();

	//void ProcessInput(const uint8_t* keyState);
	void ProcessInput(const struct InputState& state);
	//virtual void ActorInput(const uint8_t* keyState);
	virtual void ActorInput(const struct InputState& state);

	void RotateToNewForward(const Vector3& forward);

	/* ---------- Getter ---------- */
	// Transform
	const Matrix4& GetWorldTransform() const { return mWorldTransform; }
	const Vector3& GetPosition() const { return mPosition; }
	const Quaternion& GetRotation() const { return mRotation; }
	float GetScale() const { return mScale; }
	// Others
	State GetState() const { return mState; }
	class Game* GetGame() { return mGame; }
	Vector3 GetForward() const { return Vector3::Transform(Vector3::UnitX, mRotation); }
	Vector3 GetRight() const { return Vector3::Transform(Vector3::UnitY, mRotation); }
	/* ---------- Setter ---------- */
	// Transform
	void SetPosition(const Vector3& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
	void SetRotation(const Quaternion& rota) { mRotation = rota; mRecomputeWorldTransform = true; }
	void SetScale(float scale) { mScale = scale; mRecomputeWorldTransform = true; }
	// Others
	void SetState(State state) { mState = state; }

	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

private:
	State mState;

	// Transform
	Matrix4 mWorldTransform;
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
	bool mRecomputeWorldTransform;

	std::vector<class Component*> mComponents;
	class Game* mGame;
};

