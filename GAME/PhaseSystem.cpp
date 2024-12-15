#include "PhaseSystem.h"
#include "Game.h"
#include "HUD.h"

#include "EnemyActor.h"
#include "HeartActor.h"

PhaseSystem::PhaseSystem(Game* game)
	: mGame(game)
	, mCurPhase(Phases::EPhase_1)
	, mOnTransition(false)
	, mTransTime(2.0f)
	, mTimer(0.0f)
	, mEnemyGenInterval(1.0f)
{
}

PhaseSystem::~PhaseSystem()
{
}

void PhaseSystem::Update(float deltaTime)
{
	if (!mOnTransition)
	{
		mTimer += deltaTime;
		if (mTimer >= mEnemyGenInterval)
		{
			mTimer -= mEnemyGenInterval;

			//new EnemyActor(mGame);
		}
	}
	else
	{
		mTimer += deltaTime;
		if (mTimer > mTransTime)
		{
			mTimer = 0.0f;
			mOnTransition = false;
			StartPhase();
		}
	}
}

void PhaseSystem::Reset()
{
	mCurPhase = Phases::EPhase_1;
	mOnTransition = false;
}

void PhaseSystem::StartPhase()
{
	class HUD* hud = mGame->GetHUD();
	Actor* actor;
	switch (mCurPhase)
	{
	case Phases::EPhase_1:
	{
		hud->ResetTimer();
		
		actor = new EnemyActor(mGame);
		//actor = new EnemyActor(mGame);
		//actor->SetPosition(Vector3(-400.0f, 400.0f, 0.0f));
		//actor = new EnemyActor(mGame);
		//actor->SetPosition(Vector3(-300.0f, -300.0f, 0.0f));
		//actor = new EnemyActor(mGame);
		//actor->SetPosition(Vector3(300.0f, -300.0f, 0.0f));
		actor = new HeartActor(mGame);
		break;
	}
	case Phases::EPhase_2:
	{
		hud->ResetTimer();
		break;
	}
	case Phases::EPhase_3:
	{
		hud->ResetTimer();
		break;
	}
	case Phases::EPhase_Boss:
	{
		hud->ResetTimer();
		EnemyActor* eActor;
		eActor = new EnemyActor(mGame);
		eActor->SetScale(80.0f);
		eActor->SetSpeed(200.0f);
		break;
	}
	}
}

void PhaseSystem::ToNextPhase()
{
	class HUD* hud = mGame->GetHUD();
	// 次のフェーズへ移行させる
	switch (mCurPhase)
	{
	case Phases::EPhase_1:
	{
		hud->ResetTimer();
		mOnTransition = true;
		mTimer = 0.0f;
		mCurPhase = Phases::EPhase_2;
		break;
	}
	case Phases::EPhase_2:
	{
		hud->ResetTimer();
		mOnTransition = true;
		mTimer = 0.0f;
		mCurPhase = Phases::EPhase_3;
		break;
	}
	case Phases::EPhase_3:
	{
		hud->ResetTimer();
		mOnTransition = true;
		mTimer = 0.0f;
		mCurPhase = Phases::EPhase_Boss;
		break;
	}
	case Phases::EPhase_Boss:
	{
		hud->ResetTimer();
		mGame->ChangeState(Game::GameState::EGameclear);
		break;
	}
	}
}
