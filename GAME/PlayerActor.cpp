#include "PlayerActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

#include "HUD.h"

#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"
#include "FollowCamera.h"

#include "PlaneActor.h" // for collision caluculation
#include "BallActor.h"

PlayerActor::PlayerActor(Game* game)
	: Actor(game, Type::Eplayer)
	, mBoxComp(nullptr)
	, mPosState(EOnFloor)
	, mJumpSpeed(0.0f)
	, mShotInterval(2.0f)
	, mLastShot(0.0f)
	, mHP(100.0f)
	, mHUD(nullptr)
{
	mHUD = GetGame()->GetHUD();

	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	SetPosition(Vector3(0.0f, 0.0f, -50.0f));
	SetScale(100.0f);

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
	
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void PlayerActor::ActorInput(const InputState& state)
{
	float forwardSpeed = 0.0f;	// �O�i���x
	float angularSpeed = 0.0f;	// ��]���x
	// �O��Ɖ�]�̈ړ�
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_W))
	{
		forwardSpeed += 400.0f;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_S))
	{
		forwardSpeed -= 400.0f;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_A))
	{
		angularSpeed -= Math::Pi;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_D))
	{
		angularSpeed += Math::Pi;
	}
	// �W�����v
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_SPACE))
	{
		if (mPosState == EOnFloor)
		{
			mPosState = EJumping;
			mJumpSpeed = 500.0f;
		}
	}
	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);

	/* �J�����̐ݒ� */
	// �X�s�[�h�ɂ���Đ���������ς��ăX�s�[�h�����o��
	if (!Math::NearZero(forwardSpeed))
	{
		mCameraComp->SetHorzDist(500.0f);
	}
	else
	{
		mCameraComp->SetHorzDist(350.0f);
	}
}

void PlayerActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	Jump(deltaTime);
	FixCollisions();
	AutoShoot(deltaTime);
}

void PlayerActor::Reset()
{
	Actor::SetState(EActive);
	mPosState = EOnFloor;
	mJumpSpeed = 0.0f;
	mLastShot = 0.0f;
	mHP  = 100.0f;
	SetPosition(Vector3(0.0f, 0.0f, -50.0f));
}

void PlayerActor::FixCollisions()
{
	// ���[���h��Ԃ̃{�b�N�X���X�V���邽�߂ɁA������
	// ���[���h�ϊ����Čv�Z����K�v������
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	// �n�ォ�痎���ւ̑J�ڂŎg��
	LineSegment line(pos, pos + Vector3::UnitZ * -100.0f);
	float t = 0.0f;
	Vector3 norm(Vector3::Zero);
	bool slip(true);

	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		// ����PlaneActor�ƏՓ˂��邩
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			// �S�Ă̍����v�Z����
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			// dx�ɂ�dx1,dx2�̂�����Βl�������������Z�b�g����
			// dy,dz�����l
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			// x,y,z�̂����ł��������������ňʒu�𒲐�����
			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
				// ���Ƃ̏Փ˂ł���Ƃ�
				if (dz == dz1) 
				{
					//switch (mState)
					//{
					//case EOnFloor:
					//	slip = false;
					//	break;
					//case EFalling:
					//	mState = EOnFloor;
					//	break;
					//}
					if (mPosState == EFalling)
					{
						mPosState = EOnFloor;
					}
				}
			}
			// �ʒu��ݒ肵�{�b�N�X�̐������X�V����
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
			// TODO: player�̒��S�����{������line�Ńe�X�g���Ă���̂ŏC�����K�v����
			if (mPosState == EOnFloor)
			{
				if (Intersect(line, planeBox, t, norm))
				{
					slip = false;
				}
			}
		}
	}
	if (pos.z <= -300.0f)
	{
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
	if (slip && mPosState == EOnFloor)
	{
		mPosState = EFalling;
	}

}

void PlayerActor::TakeDamage(float amount)
{
	// TODO: �������Ă��琔�t���[��,�F���ς��悤�ɂ�����ʔ�������
	mHP -= amount;
	mHUD->SetHPdiscardRange(mHP / 100.0f);
	if (mHP <= 0.0f)
	{
		//GetGame()->SetState(Game::GameState::EGameover);
		//GetGame()->OnChangeState(Game::GameState::EGameover);
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
}

void PlayerActor::Jump(float deltaTime)
{
	if (mPosState == EJumping || mPosState == EFalling)
	{
		Vector3 pos = GetPosition();
		pos += Vector3::UnitZ * mJumpSpeed * deltaTime;
		mJumpSpeed -= 1000.0f * deltaTime;

		if (mPosState == EJumping && mJumpSpeed < 0.0f)
		{
			mPosState = EFalling;
		}
		//else if (mState == EFalling)
		//{
		//}

		SetPosition(pos);
	}
}

void PlayerActor::AutoShoot(float deltaTime)
{
	mLastShot += deltaTime;
	if (mLastShot > mShotInterval)
	{
		mLastShot -= mShotInterval;
		/* ���̈ʒu�A���������߂� �֐������� */
		Vector3 start = GetPosition();
		Vector3 dir = GetForward();
		// �����쐬���ĐF�X�ݒ�
		BallActor* ball = new BallActor(GetGame());
		ball->SetPlayer(this);
		ball->SetPosition(start + dir * 50.0f);
		ball->RotateToNewForward(dir);
		// �T�E���h
		//mAudioComp->PlayEvent("event:/Shot");
	}
}
