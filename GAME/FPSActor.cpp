#include "SDL/SDL_scancode.h"
#include "Game.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "MeshComponent.h"
#include "FPSCamera.h"
#include "BoxComponent.h"
#include "FPSActor.h"
#include "PlaneActor.h"
#include "BallActor.h"
#include "Mesh.h"

FPSActor::FPSActor(Game* game)
	: Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);

	mCameraComp = new FPSCamera(this);

	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));

	mBoxComp = new BoxComponent(this);
	// 50�~50�~175
	AABB myBox(Vector3(-25.0f, -25.0f, -87.5f), Vector3(25.0f, 25.0f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

	mLastFootstep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	// FPS���f���̓A�N�^�[�Ƃ̑��Έʒu�ɂ��X�V
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);
	// �A�N�^�[�̉�]�ŏ�����
	Quaternion q = GetRotation();
	// �J�����̃s�b�`�ɂ���]
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
}

//void FPSActor::ActorInput(const uint8_t* keys)
//{
//	float forwardSpeed = 0.0f;
//	float strafeSpeed = 0.0f;
//
//	if (keys[SDL_SCANCODE_W])
//	{
//		forwardSpeed += 400.0f;
//	}
//	if (keys[SDL_SCANCODE_S])
//	{
//		forwardSpeed -= 400.0f;
//	}
//	if (keys[SDL_SCANCODE_A])
//	{
//		strafeSpeed -= 400.0f;
//	}
//	if (keys[SDL_SCANCODE_D])
//	{
//		strafeSpeed += 400.0f;
//	}
//
//	mMoveComp->SetForwardSpeed(forwardSpeed);
//	mMoveComp->SetStrafeSpeed(strafeSpeed);
//
//	/* �}�E�X�ɂ��p���x�̌v�Z */
//	// SDL���瑊�Ή^�����擾
//	int x, y;
//	Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
//	const int maxMouseSpeed = 500;
//	const float maxAngularSpeed = Math::Pi * 8;
//	float angularSpeed = 0.0f;
//	if (x != 0)
//	{
//		// ���悻[-1.0, 1.0]�͈̔͂ɕϊ�
//		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
//		// �ő�ړ��ʂł̊p���x��������
//		angularSpeed *= maxAngularSpeed;
//	}
//	mMoveComp->SetAngularSpeed(angularSpeed);
//
//	// �s�b�`�̌v�Z
//	const float maxPitchSpeed = Math::Pi * 8;
//	float pitchSpeed = 0.0f;
//	if (y != 0)
//	{
//		// ���悻[-1.0, 1.0]�͈̔͂ɕϊ�
//		pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
//		// �ő�ړ��ʂł̊p���x��������
//		pitchSpeed *= maxPitchSpeed;
//	}
//	mCameraComp->SetPitchSpeed(pitchSpeed);
//}

void FPSActor::Shoot()
{
	/* ���̈ʒu�A���������߂� �֐������� */
	//Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	//Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	//screenPoint.z = 0.9f;
	//Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	//Vector3 dir = end - start;
	//dir.Normalize();
	Vector3 start, dir;
	GetGame()->GetRenderer()->GetScreenDirection(start, dir);

	// �����쐬���ĐF�X�ݒ�
	BallActor* ball = new BallActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.0f);
	ball->RotateToNewForward(dir);
	// �T�E���h
	mAudioComp->PlayEvent("event:/Shot");
}

void FPSActor::SetFootstepSurface(float value)
{
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}

void FPSActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

void FPSActor::FixCollisions()
{
	// ���[���h��Ԃ̃{�b�N�X���X�V���邽�߂ɁA������
	// ���[���h�ϊ����Čv�Z����K�v������
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

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
			if(Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
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
			}

			// �ʒu��ݒ肵�{�b�N�X�̐������X�V����
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}
