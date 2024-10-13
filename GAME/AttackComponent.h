#pragma once
#include "Component.h"

class AttackComponent :
    public Component
{
public:
    AttackComponent(class Actor* owner);
    void Damage(class Actor* target);

    void SetAttackDamage(float damage) { mAttackDamage = damage; }

private:
    float mAttackDamage;    // �U���́I��������actor�ɂ���ĕύX�\()
};

// �ŏ���AttackDamage���A�N�^�[���g�Ɏ������Ă�����Component�ɓ���ꂻ���Ȃ�
// ���̕����������肷��̂œ��ꂽ