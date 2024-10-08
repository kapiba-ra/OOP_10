#include "FollowActor.h"
#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"
#include "FollowCamera.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
// for collision caluculation
#include "PlaneActor.h"

FollowActor::FollowActor(Game* game)
	: Actor(game)
	, mBoxComp(nullptr)
	, mState(EOnFloor)
	, mJumpSpeed(0.0f)
{
	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	SetPosition(Vector3(0.0f, 0.0f, -50.0f));
	SetScale(100.0f);

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
	
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f,0.5f,0.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FollowActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;	// �O�i���x
	float angularSpeed = 0.0f;	// ��]���x
	// �O��Ɖ�]�̈ړ�
	if (keys[SDL_SCANCODE_W])
	{
		forwardSpeed += 400.0f;
	}
	if (keys[SDL_SCANCODE_S])
	{
		forwardSpeed -= 400.0f;
	}
	if (keys[SDL_SCANCODE_A])
	{
		angularSpeed -= Math::Pi;
	}
	if (keys[SDL_SCANCODE_D])
	{
		angularSpeed += Math::Pi;
	}
	// �W�����v
	if (keys[SDL_SCANCODE_SPACE])
	{
		if (mState == EOnFloor)
		{
			mState = EJumping;
			SetJumpSpeed(500.0f);
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

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();
	Jump(deltaTime);
}

void FollowActor::FixCollisions()
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
					switch (mState)
					{
					case EOnFloor:
						slip = false;
						break;
					case EFalling:
						mState = EOnFloor;
						break;
					}
				}
			}
			// �ʒu��ݒ肵�{�b�N�X�̐������X�V����
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
	if (slip && mState == EOnFloor)
	{
		mState = EFalling;
	}
}

void FollowActor::Jump(float deltaTime)
{
	if (mState == EJumping)
	{
		Vector3 pos = GetPosition();
		pos += Vector3::UnitZ * mJumpSpeed * deltaTime;
		mJumpSpeed -= 1000.0f * deltaTime;
		if (mJumpSpeed < 0.0f)
		{
			mState = EFalling;
		}
		SetPosition(pos);
	}
	else if (mState == EFalling)
	{
		Vector3 pos = GetPosition();
		pos += Vector3::UnitZ * mJumpSpeed * deltaTime;
		mJumpSpeed -= 1000.0f * deltaTime;

		SetPosition(pos);
	}
}
