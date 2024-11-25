#pragma once
#include "Actor.h"
#include <unordered_map>
#include <string>

class PlayerActor :
	public Actor
{
public:
	PlayerActor(class Game* game);
	~PlayerActor();

	void ActorInput(const struct InputState& state) override;
	void UpdateActor(float deltaTime) override;
	void Reset() override;

	// 2��AABB�̍ŏ��̏d�Ȃ�e�X�g���s��
	void FixCollisions();

	void GainExp(float exp);

	class BoxComponent* GetBox() { return mBoxComp; }
	class HpComponent* GetHpComp() { return mHpComp; }
	// ��ŏ���
	class ShotComponent* GetShotComp() { return mShotComp; }
	void AddWeapon(std::string name, class WeaponComponent* weapon);
	// name�͂�����Q�ƂɂȂ�(enum��skill�̃^�C�v�𕪂���)
	void LevelUpWeapon(std::string name, int lv);
	// ���N���X���������͉�����Ă����̂łƂ肠�����v��Ȃ�
	//void RemoveWeapon(WeaponComponent* weapon);
	float GetForwardSpeed();

private:
	void CheckLevelUp();

	class HUD* mHUD;
	// �X�e�[�^�X,�p�����[�^�Ɋ֘A�������
	class MoveComponent* mMoveComp;
	class JumpComponent* mJumpComp;
	class HpComponent* mHpComp;
	// essential
	class FollowCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class BoxComponent* mBoxComp;
	class AudioComponent* mAudioComp;
	// weapon
	std::unordered_map<std::string, class WeaponComponent*> mWeapons;
	class ShotComponent* mShotComp;

public:
	// parameter related
	struct Parameters
	{
		float maxForwardSpeed = 400.0f;
		float maxJumpSpeed = 500.0f;
		float WeaponSizeFactor = 1.0f;
		float WeaponIntervalFactor= 1.0f;
		float WeaponSpeedFactor = 1.0f;

		float expToLevelUp = 1.0f;
		float exp = 0.0f;
		int level = 1;

		void Reset();
	};
	Parameters& GetParams() { return mParams; }

private:
	Parameters mParams;
};

