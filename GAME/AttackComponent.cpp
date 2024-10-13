#include "AttackComponent.h"

//#include "Actor.h"
#include "EnemyActor.h"
#include "FollowActor.h"

AttackComponent::AttackComponent(Actor* owner)
	: Component(owner)
	, mAttackDamage(1.0f)
{
}

void AttackComponent::Damage(Actor* target)
{
	// �Ώۂ�Actor���󂯎��Adamage�̕�����hp�����炷
	// ���݂�Actor���g��hp���������Ă�������炵�Ă��邪�Ahp��
	// ���̃X�e�[�^�X���Ǘ����邽�߂�Component��p�ӂ��āA�����ɓn���悤��
	// ��������������������Ȃ��B�����Ă��Ďv���������̏ꍇ�ł������̏�����
	// ����Ȃɕς��Ȃ������H(TakeDamage������������)
	switch (target->GetType())
	{
	case Actor::Type::Eenemy:
	{
		static_cast<EnemyActor*>(target)->TakeDamage(mAttackDamage);
		break;
	}
	case Actor::Type::Eplayer:
	{
		static_cast<FollowActor*>(target)->TakeDamage(mAttackDamage);
		break;
	}
	}
}
