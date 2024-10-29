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
		EJumping,	// falling�ƕ����Ă邯�ǌ���͊��p���ĂȂ�
		EFalling
	};
	PosState GetPosState() const { return mPosState; }
	void SetPosState(PosState state) { mPosState = state; }

	// 2��AABB�̍ŏ��̏d�Ȃ�e�X�g���s��
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

	// parameter�Ƃ��Ă܂Ƃ߂�ꂻ��
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
	// Data�N���X���珉��������������
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
		// shot�n�̋@�\��component�ɂ܂Ƃ߂�ׂ�����(�G�����Ȃ�)
		int shotNum = 1;
		float lastShot = 0.0f;
		float shotInterval = 1.0f;

		void Reset();
	};
	// ���������\�ɂ��Ă���
	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

