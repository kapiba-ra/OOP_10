#pragma once
#include "Component.h"

/// <summary>
/// �_���[�W��^����d�g�݂�S������
/// �U���͂ɓ�����p�����[�^������
/// </summary>
class DamageComponent :
    public Component
{
public:
    DamageComponent(class Actor* owner);
    void Damage(class Actor* target);

    void SetAttackDamage(float damage) { mDamageAmt = damage; }

private:
    float mDamageAmt;    // �U���́I��������actor�ɂ���ĕύX�\()
};

// �ŏ���DamageAmt���A�N�^�[���g�Ɏ������Ă�����Component�ɓ���ꂻ���Ȃ�
// ���̕����������肷��̂œ��ꂽ