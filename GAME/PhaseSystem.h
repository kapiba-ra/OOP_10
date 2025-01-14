#pragma once

// タワーディフェンスのフェーズのイメージ
// ６０秒経過毎にPhase何とか…って表示させたり、時間戻したりするだけ
// 将来的に複数ステージ作るならそれに統合してもいいかも

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

	float GetMaxPhaseTime() const { return mMaxPhaseTime; }
	float GetPhaseTimer() const { return mPhaseTimer; }
	int GetPhaseNum() const { return mPhaseNum; }
	bool OnTransition() const { return mOnTransition; }

private:

	class Game* mGame;
	Phases mCurPhase;

	bool mOnTransition;		// Phase移行中
	float mTransTime;		// Phase移行時間
	float mEnemyGenTimer;	// 敵出現用のタイマー
	float mEnemyGenInterval;// 敵の出現間隔
	float mPhaseTimer;		// Phase用のタイマー
	float mMaxPhaseTime;	// 1Phaseの時間
	int mPhaseNum;			// 何フェーズ目かをintで
};
