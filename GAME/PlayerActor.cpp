#include "PlayerActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

#include "HUD.h"
//#include "LevelUpMenu.h"

#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"
#include "FollowCamera.h"
#include "ShotComponent.h"

#include "PlaneActor.h" // for collision caluculation
#include "BallActor.h"
#include "ItemActor.h"

PlayerActor::PlayerActor(Game* game)
	: Actor(game, Type::Eplayer)
	, mBoxComp(nullptr)
	, mPosState(EOnFloor)
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

	mShotComp = new ShotComponent(this);
}

void PlayerActor::ActorInput(const InputState& state)
{
	float forwardSpeed = 0.0f;	// �O�i���x
	float angularSpeed = 0.0f;	// ��]���x
	// �O��Ɖ�]�̈ړ�
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_W))
	{
		forwardSpeed += mParams.maxForwardSpeed;
	}
	if (state.Keyboard.GetKeyValue(SDL_SCANCODE_S))
	{
		forwardSpeed -= mParams.maxForwardSpeed;
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
			mParams.jumpSpeed = 500.0f;
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
}

void PlayerActor::Reset()
{
	Actor::SetState(EActive);
	mPosState = EOnFloor;
	mParams.Reset();

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
	bool inAir(true); // �󒆂ɂ��邩
	
	float t = 0.0f;				 // �����ݒ�p�ɕ֋X��K�v,�g��Ȃ�
	Vector3 norm(Vector3::Zero); // ����

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
			// �����͗����p�̏���
			// TODO: player�̒��S�����{������line�Ńe�X�g���Ă���̂ŏC�����K�v����
			if (mPosState == EOnFloor)
			{
				if (Intersect(line, planeBox, t, norm))
				{
					inAir = false;
				}
			}
		}
	}

	// ��藎��������gameover��
	if (pos.z <= -300.0f)
	{
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
	// inAir�Ȃ�Efalling��
	else if (inAir && mPosState == EOnFloor)
	{
		mPosState = EFalling;
	}

	// �A�C�e���擾�p
	auto& items = GetGame()->GetItems();
	for (auto item : items)
	{
		Vector3 itemPos = item->GetPosition();
		Vector3 dir = itemPos - pos;
		float getRadius = 200.0f;
		if (dir.Length() < getRadius)
		{
			item->OnAcquired();
		}
	}
}

void PlayerActor::TakeDamage(float amount)
{
	// TODO: �������Ă��琔�t���[��,�F���ς��悤�ɂ�����ʔ�������
	mParams.hp -= amount;
	if (mParams.hp <= 0.0f)
	{
		Actor::SetState(EPaused);
		GetGame()->ChangeState(Game::EGameover);
	}
}

void PlayerActor::GainExp(float exp)
{
	mParams.exp += exp;
	CheckLevelUp();
}

void PlayerActor::Jump(float deltaTime)
{
	if (mPosState == EJumping || mPosState == EFalling)
	{
		Vector3 pos = GetPosition();
		pos += Vector3::UnitZ * mParams.jumpSpeed * deltaTime;
		mParams.jumpSpeed -= 1000.0f * deltaTime;

		if (mPosState == EJumping && mParams.jumpSpeed < 0.0f)
		{
			mPosState = EFalling;
		}

		SetPosition(pos);
	}
}

void PlayerActor::CheckLevelUp()
{
	if (mParams.exp > mParams.expToLevelUp)
	{
		mParams.level += 1;
		mParams.exp -= mParams.expToLevelUp;
		mParams.expToLevelUp += 1.0f;	// ���͓K��
		GetGame()->ChangeState(Game::ELevelUp);
	}
}

void PlayerActor::OnLvUpSkill(const std::string& name)
{
	// ������ł���Ă�̂����傢�s��
	if (name == "MaxHp")
	{
		mParams.maxHp += 20;
		mParams.hp += 20;
	}
	else if (name == "PlayerSpeed")
	{
		mParams.maxForwardSpeed += 50.0f;
	}
	else if (name == "ShotSize")
	{
		float scale = mShotComp->GetBallScale();
		scale += 0.2f;
		mShotComp->SetBallScale(scale);
	}
	else if (name == "ShotNum")
	{
		int shotNum = mShotComp->GetShotNum();
		shotNum += 1;
		mShotComp->SetShotNum(shotNum);
	}
	else if (name == "ShotInterval")
	{
		float interval = mShotComp->GetShotInterval();
		interval *= 0.9f;
		mShotComp->SetShotInterval(interval);
	}
	else if (name == "ShotSpeed")
	{
		//float speed = mShotComp->
	}
}

void PlayerActor::Parameters::Reset()
{
	maxForwardSpeed = 400.0f;
	jumpSpeed = 0.0f;
	hp = 100.0f;
	exp = 0.0f;
	expToLevelUp = 1.0f;
	level = 1;
}
