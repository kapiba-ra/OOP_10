#include "BallMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "BallActor.h"
#include "TargetActor.h"

BallMove::BallMove(Actor* owner)
	: MoveComponent(owner)
	, mPlayer(nullptr)
{
}

void BallMove::Update(float deltaTime)
{
	// �i�s�����̐������\�z
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition(); // �n�_
	Vector3 dir = mOwner->GetForward();	   // �i�s����
	Vector3 end = start + dir * segmentLength; // �I�_
	LineSegment ls(start, end);

	// �����ƃ��[���h�̏Փ˂𔻒�
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// Player�Ƃ͏Փ˂��Ȃ��悤�ɂ��Ă���
	if (phys->SegmentCast(ls, info) && info.mActor!= mPlayer)
	{
		// �Փ˂�����@���̌����ŕ����𔽎˂�����
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);
		// �^�[�Q�b�g�Ƀq�b�g������
		TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
		if (target)
		{
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}

	// ���N���X�œ������X�V����
	MoveComponent::Update(deltaTime);
}