#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	: CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f) // 60�x
	, mPitch(0.0f)
{
}

void FPSCamera::Update(float deltaTime)
{
	/* �s�b�`�Ȃ��̃J���� */
	//// �J�����̈ʒu�͏��L�A�N�^�[�̈ʒu
	//Vector3 cameraPos = mOwner->GetPosition();
	//// �^�[�Q�b�g�̈ʒu�͏��L�A�N�^�[�̑O��100�P��
	//Vector3 target = cameraPos + mOwner->GetForward() * 100.0f;
	//// ����x�N�g���͏��z���̊�{�x�N�g��
	//Vector3 up = Vector3::UnitZ;
	//// �����s�������ăr���[�ɐݒ肷��
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	//SetViewMatrix(view);

	/* �s�b�`���� */
	CameraComponent::Update(deltaTime); \

	// �J�����̈ʒu�͏��L�A�N�^�[�̈ʒu
	Vector3 cameraPos = mOwner->GetPosition();
	// �s�b�`�̊p���x�Ɋ�Â��ăs�b�`���X�V
	mPitch += mPitchSpeed * deltaTime;
	// �s�b�`�͈̔͂𐧌�
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// �s�b�`��]��\���N�H�[�^�j�I�������
	// (�I�[�i�[�̉E�����x�N�g�������Ƃ����])
	Quaternion q(mOwner->GetRight(), mPitch);
	// �s�b�`�̃N�H�[�^�j�I���ŏ��L�A�N�^�[�̑O���x�N�g������]
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	// �^�[�Q�b�g�̈ʒu�͏��L�A�N�^�[�̑O��100�P��
	Vector3 target = cameraPos + viewForward * 100.0f;
	// ����x�N�g�����s�b�`�̃N�H�[�^�j�I���ŉ�]
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// �����s�������ăr���[�ɐݒ�
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}