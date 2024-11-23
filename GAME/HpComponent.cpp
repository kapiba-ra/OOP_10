#include "HpComponent.h"

HpComponent::HpComponent(Actor* owner, float maxHp)
	: Component(owner)
	, mMaxHp(maxHp)
	, mCurHp(maxHp)
{
}

void HpComponent::Update(float deltaTime)
{
}

void HpComponent::Reset(float initialHp)
{
	mCurHp = initialHp;
	mMaxHp = initialHp;
}

void HpComponent::TakeDamage(float damage)
{
	// hp��0�ȉ��ɂȂ�Ȃ�
	float diff = mCurHp - damage;
	if (diff <= 0.0f)
	{
		damage = mCurHp;
	}
	mCurHp -= damage;
}

void HpComponent::Recover(float recover)
{
	// hp��mMaxHp�𒴂��Ȃ�
	float diff = mMaxHp - mCurHp;
	if (diff <= recover)
	{
		recover = diff;
	}
	mCurHp += recover;
}

void HpComponent::AddMaxHp(float add)
{
	mMaxHp += add;
	mCurHp += add;
}
