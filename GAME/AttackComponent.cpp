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
	// 対象のActorを受け取り、damageの分だけhpを減らす
	// 現在はActor自身にhpを持たせてそれを減らしているが、hpや
	// 他のステータスを管理するためのComponentを用意して、そこに渡すように
	// した方がいいかもしれない。書いていて思ったがその場合でもここの処理は
	// そんなに変わらないかも？(TakeDamageを書き換える)
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
