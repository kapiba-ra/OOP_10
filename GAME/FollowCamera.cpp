#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
	: CameraComponent(owner)
	//, mHorzDist(350.0f)
	//, mVertDist(150.0f)
	, mHorzDist(100.0f)
	, mVertDist(1000.0f)
	, mTargetDist(100.0f)
	, mSpringConstant(64.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// �΂˒萔���猸�����v�Z
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);

	// ���z�̈ʒu���v�Z
	Vector3 idealPos = ComputeCameraPos();

	// ���ۂƗ��z�̍����v�Z
	Vector3 diff = mActualPos - idealPos;
	// �΂˂ɂ������x���v�Z
	Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;

	// ���x�̍X�V
	mVelocity += acel * deltaTime;
	// ���ۂ̃J�����|�W�V�������X�V
	mActualPos += mVelocity * deltaTime;
	// �^�[�Q�b�g�͏��L�A�N�^�[����O���ɗ��ꂽ���W
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	// ���z�ł͂Ȃ����ۂ̃|�W�V�������g��
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetViewMatrix(view);
}

void FollowCamera::SnapToIdeal()
{
	// ���ۂ̈ʒu�͗��z�̈ʒu�Ɠ���
	mActualPos = ComputeCameraPos();
	mVelocity = Vector3::Zero;
	// �����_�ƃr���[���v�Z
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// �J�����̓A�N�^�[�̏����ɂ���悤�ɂ���
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}
