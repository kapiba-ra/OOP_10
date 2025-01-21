#include "PhaseSystem.h"
#include "Game.h"
#include "AudioSystem.h"
#include "HUD.h"

#include "EnemyActor.h"
#include "HeartActor.h"

PhaseSystem::PhaseSystem(Game* game)
	: mGame(game)
	, mCurPhase(Phases::EPhase_1)
	, mOnTransition(false)
	, mTransTime(2.0f)
	, mEnemyGenTimer(0.0f)
	, mEnemyGenInterval(1.0f)
	, mPhaseTimer(0.0f)
	, mMaxPhaseTime(60.0f)
	, mPhaseNum(1)
{
}

PhaseSystem::~PhaseSystem()
{
}

void PhaseSystem::Update(float deltaTime)
{
	// Phase進行中の処理,敵を生み出す
	// 難易度調節を行うのはここ
	if (!mOnTransition)
	{
		mEnemyGenTimer += deltaTime;
		mPhaseTimer += deltaTime;
		if (mEnemyGenTimer >= mEnemyGenInterval)
		{
			mEnemyGenTimer -= mEnemyGenInterval; // タイマーリセット
			EnemyActor* eActor = nullptr;
			switch (mCurPhase)
			{
			case Phases::EPhase_1:
			{
				eActor = new Slime(mGame);
				break;
			}
			case Phases::EPhase_2:
			{
				eActor = new Zombie(mGame);
				eActor->SetMaxHp(2.0f);
				break;
			}
			case Phases::EPhase_3:
			{
				eActor = new Slime(mGame);
				eActor->SetSpeed(eActor->GetForwardSpeed() * 1.2f);
				eActor = new Zombie(mGame);
				eActor->SetMaxHp(2.0f);
				break;
			}
			case Phases::EPhase_Boss:
			{
				eActor = new Slime(mGame);
				eActor->SetSpeed(eActor->GetForwardSpeed() * 1.3f);
				eActor = new Zombie(mGame);
				eActor->SetMaxHp(2.0f);
				eActor->SetSpeed(eActor->GetForwardSpeed() * 1.2f);
				break;
			}
			}
		}
		if (mPhaseTimer >= mMaxPhaseTime + deltaTime) // 60を超えさせたいので(UIの都合),deltaTimeを+している
		{
			ToNextPhase();
		}
	}
}

void PhaseSystem::Reset()
{
	mCurPhase = Phases::EPhase_1;
	StartPhase();
	mOnTransition = false;
	mEnemyGenTimer = 0.0f;
	mPhaseTimer = 0.0f;
	mPhaseNum = 1;
}

void PhaseSystem::StartPhase()
{
	mOnTransition = false;
	EnemyActor* eActor;

	switch (mCurPhase)
	{
	case Phases::EPhase_1:
	{
		new HeartActor(mGame);
		break;
	}
	case Phases::EPhase_2:
	{
		new HeartActor(mGame);
		break;
	}
	case Phases::EPhase_3:
	{
		// Phase3は敵発生間隔を2秒にする
		mEnemyGenInterval = 2.0f;
		new HeartActor(mGame);
		break;
	}
	case Phases::EPhase_Boss:
	{
		// Phase3は敵発生間隔を2秒にする
		mEnemyGenInterval = 2.0f;
		// Boss
		eActor = new Zombie(mGame);
		eActor->SetScale(200.0f);
		eActor->SetSpeed(200.0f);
		eActor->SetMaxHp(50.0f);
		eActor->SetExp(20.0f);
		break;
	}
	}
}

void PhaseSystem::ToNextPhase()
{
	mOnTransition = true;	// Phase移行中,の状態へ
	mPhaseTimer = 0.0f;		// タイマーリセット
	mEnemyGenTimer = 0.0f;	// タイマーリセット
	mPhaseNum += 1;		// 現在のPhase数をプラス
	
	// 音を鳴らす
	mGame->GetAudioSystem()->PlayEvent("event:/Explosion2D");

	HUD* hud = mGame->GetHUD();
	hud->ToNextPhase();

	// 次のフェーズへ移行させる
	switch (mCurPhase)
	{
	case Phases::EPhase_1:
	{
		mCurPhase = Phases::EPhase_2;
		break;
	}
	case Phases::EPhase_2:
	{
		mCurPhase = Phases::EPhase_3;
		break;
	}
	case Phases::EPhase_3:
	{
		mCurPhase = Phases::EPhase_Boss;
		break;
	}
	case Phases::EPhase_Boss:
	{
		mGame->ChangeState(Game::GameState::EGameclear);
		break;
	}
	}
}
