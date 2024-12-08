#include "BallMove.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "BallActor.h"

BallMove::BallMove(Actor* owner)
	: MoveComponent(owner)
{
}

void BallMove::Update(float deltaTime)
{
	// �i�s�����̐������\�z
	const float segmentLength = 30.0f;
	Vector3 start = mOwner->GetPosition();	// �n�_
	Vector3 dir = mOwner->GetForward();		// �i�s����
	Vector3 end = start + dir * segmentLength; // �I�_
	LineSegment ls(start, end);

	// �����ƃ��[���h�̏Փ˂𔻒�
	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	// Player,Weapon�Ƃ͏Փ˂��Ȃ��悤�ɂ��Ă���
	if (phys->SegmentCast(ls, info) && 
		(info.mActor->GetType() == Actor::Eenemy ||
		info.mActor->GetType() == Actor::Eplane) 
		)
	{
		// �Փ˂�����@���̌����ŕ����𔽎˂�����
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);
		/* �^�[�Q�b�g�Ƀq�b�g������ */
		if (info.mActor->GetType() == Actor::Type::Eenemy)
		{
			mOwner->SetState(Actor::State::EDead); // erase ball
			static_cast<BallActor*>(mOwner)->HitTarget(info.mActor);
		}
	}

	// ���N���X�œ������X�V����
	MoveComponent::Update(deltaTime);
}

// SegmentCast�͐����ƃ��[���h�̑S�Ẵ{�b�N�X�Ƃ̃e�X�g���s��
// ���̐݌v�ł͓��������{�b�N�X�̎������m�邽�߂�,
// info.mActor��h���N���X�̃|�C���^��dynamic_cast���Ă����B
// ����,RTTI�Ɉˑ������ɐ݌v����悤��,���N���X�ɔh���N���X�̏���
// Type�Ƃ��Ď�����,Type���画�f�ł���悤�ɂ����B