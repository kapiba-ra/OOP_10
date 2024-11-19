#pragma once
#include <vector>
#include <cstdint>
#include "Math.h"

/// <summary>
/// ゲーム中の全てのオブジェクトの基底クラス
/// Transform,状態管理,アタッチする全Componentを格納した配列等を持つ
/// </summary>
class Actor
{
public:
	// 競合しないように注意(しそうならenum classにする)
	// Eのあと小文字or大文字で一応分けている
	enum State
	{
		EActive,
		EPaused,
		EDead
	};
	enum Type
	{
		Enone,
		Eplane,
		Eplayer,
		Eenemy,
		Etarget,
		Eitem,
		Eball
	};

	Actor(class Game* game, Type type = Type::Enone);
	virtual ~Actor();

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime);
	void ComputeWorldTransform();

	void ProcessInput(const struct InputState& state);
	virtual void ActorInput(const struct InputState& state);

	virtual void Reset();

	void RotateToNewForward(const Vector3& forward);

	/* ---------- Getter ---------- */
	// Transform
	const Matrix4& GetWorldTransform() const { return mWorldTransform; }
	const Vector3& GetPosition() const { return mPosition; }
	const Quaternion& GetRotation() const { return mRotation; }
	float GetScale() const { return mScale; }
	// Others
	State GetState() const { return mState; }
	Type GetType() const { return mType; }
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
	Type mType;

	// Transform
	Matrix4 mWorldTransform;
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
	bool mRecomputeWorldTransform;

	std::vector<class Component*> mComponents;
	class Game* mGame;
};

