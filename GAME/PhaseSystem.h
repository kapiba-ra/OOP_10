#pragma once

// �^���[�f�B�t�F���X�̃t�F�[�Y�̃C���[�W
// ��Ȗ��O�����A�U�O�b�o�ߖ���Phase���Ƃ��c���ĕ\����������A���Ԗ߂����肷�邾��
// �����I�ɕ����X�e�[�W���Ȃ炻��ɓ������Ă���������

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

	void ToNextPhase();
	void StartPhase();

private:

	class Game* mGame;
	Phases mCurPhase;

	bool mOnTransition;
	float mTransTime;
	float mTimer;
	float mEnemyGenInterval;
};

