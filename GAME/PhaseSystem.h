#pragma once

// タワーディフェンスのフェーズのイメージ
// 大仰な名前だが、６０秒経過毎にPhase何とか…って表示させたり、時間戻したりするだけ
// 将来的にはステージ側からステージ毎にPhaseの数とかを設定できるようにすればいいかも

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
};

