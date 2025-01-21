#pragma once

// �^���[�f�B�t�F���X�̃t�F�[�Y�̃C���[�W
// �U�O�b�o�ߖ���Phase���Ƃ��c���ĕ\����������A���Ԃ�߂����肷��
// �����I�ɕ����X�e�[�W���Ȃ炻�����ɓ������Ă���������
// ��Փx���߂��s���̂����̃N���X
class PhaseSystem
{
public:
	PhaseSystem(class Game* game);
	~PhaseSystem();

	enum class Phases
	{
		EPhase_1,
		EPhase_2,
		EPhase_3,
		EPhase_Boss,
	};

	void Update(float deltaTime);

	void Reset();
	// �O������Ăяo��(����HUD�N���X����)
	void StartPhase();

	float GetMaxPhaseTime() const { return mMaxPhaseTime; }
	float GetPhaseTimer() const { return mPhaseTimer; }
	int GetPhaseNum() const { return mPhaseNum; }
	bool OnTransition() const { return mOnTransition; }

private:
	void ToNextPhase();

	class Game* mGame;
	Phases mCurPhase;

	bool mOnTransition;		// Phase�ڍs��
	float mTransTime;		// Phase�ڍs����
	float mEnemyGenTimer;	// �G�o���p�̃^�C�}�[
	float mEnemyGenInterval;// �G�̏o���Ԋu
	float mPhaseTimer;		// Phase�p�̃^�C�}�[
	float mMaxPhaseTime;	// 1Phase�̎���
	int mPhaseNum;			// ���t�F�[�Y�ڂ���int��
};
