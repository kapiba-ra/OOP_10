#include "DamageComponent.h"

//#include "Actor.h"
#include "EnemyActor.h"
#include "PlayerActor.h"

#include "HpComponent.h"

DamageComponent::DamageComponent(Actor* owner)
	: Component(owner)
	, mDamageAmt(1.0f)
{
}

void DamageComponent::Damage(Actor* target)
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
		static_cast<EnemyActor*>(target)->TakeDamage(mDamageAmt);
		break;
	}
	case Actor::Type::Eplayer:
	{
		static_cast<PlayerActor*>(target)->GetHpComp()->TakeDamage(mDamageAmt);
		break;
	}
	}
}
