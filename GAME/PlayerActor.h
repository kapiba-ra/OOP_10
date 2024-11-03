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

	enum PosState
	{
		EOnFloor,
		EJumping,	// falling�ƕ����Ă邯�ǌ���͊��p���ĂȂ�
		EFalling
	};
	PosState GetPosState() const { return mPosState; }
	void SetPosState(PosState state) { mPosState = state; }

	// 2��AABB�̍ŏ��̏d�Ȃ�e�X�g���s��
	void FixCollisions();

	void TakeDamage(float amount);

	void GainExp(float exp);
	void GainHeart(float recover);

	class BoxComponent* GetBox() { return mBoxComp; }

private:
	PosState mPosState;

	void Jump(float deltaTime);
	void CheckLevelUp();

	class HUD* mHUD;
	class MoveComponent* mMoveComp;
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	class ShotComponent* mShotComp;

public:
	// parameter related
	// Data�N���X���珉��������������
	struct Parameters
	{
		float maxForwardSpeed = 400.0f;
		float jumpSpeed = 0.0f;
		float maxHp = 100.0f;
		float hp = maxHp;
		float exp = 0.0f;
		float expToLevelUp = 1.0f;
		int level = 1;

		void Reset();
	};

	void OnLvUpSkill(const std::string& name);

	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

