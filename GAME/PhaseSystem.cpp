#include "PhaseSystem.h"
#include "Game.h"
#include "AudioSystem.h"
#include "HUD.h"

#include "EnemyActor.h"
#include "HeartActor.h"
#include "HpComponent.h"

PhaseSystem::PhaseSystem(Game* game)
	: mGame(game)
	, mCurPhase(Phases::EPhase_1)
	, mOnTransition(false)
	, mTransTime(2.0f)
	, mEnemyGenTimer(0.0f)
	, mEnemyGenInterval(2.0f)
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
	// 通常時
	if (!mOnTransition)
	{
		mEnemyGenTimer += deltaTime;
		mPhaseTimer += deltaTime;
		if (mEnemyGenTimer >= mEnemyGenInterval)
		{
			mEnemyGenTimer -= mEnemyGenInterval;

			new Slime(mGame);
			new Zombie(mGame);
		}
		if (mPhaseTimer >= mMaxPhaseTime + deltaTime) // 60を超えさせたいので(UIの都合),deltaTimeを+している
		{
			ToNextPhase();
		}
	}
	// Phase切り替え中
	//else
	//{
	//	mPhaseTimer += deltaTime;
	//	if (mPhaseTimer > mTransTime)
	//	{
	//		mPhaseTimer = 0.0f;
	//		mOnTransition = false;
	//		StartPhase();
	//	}
	//}
}

void PhaseSystem::Reset()
{
	mCurPhase = Phases::EPhase_1;
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
		
		eActor = new Slime(mGame);
		eActor->GetHpComp()->SetMaxHp(2.0f);
		eActor = new Slime(mGame);
		eActor->SetPosition(Vector3(-400.0f, 400.0f, -100.0f));
		eActor = new Slime(mGame);
		eActor->SetPosition(Vector3(-300.0f, -300.0f, 0.0f));
		eActor = new Slime(mGame);
		eActor->SetPosition(Vector3(300.0f, -300.0f, 0.0f));
		eActor->SetScale(200.0f);
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
		new HeartActor(mGame);
		break;
	}
	case Phases::EPhase_Boss:
	{
		eActor = new Zombie(mGame);
		eActor->SetScale(200.0f);
		eActor->SetSpeed(200.0f);
		eActor->GetHpComp()->SetMaxHp(50.0f);
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
