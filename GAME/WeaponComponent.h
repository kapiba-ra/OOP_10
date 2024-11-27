#pragma once
#include "Component.h"

/// <summary>
/// ������A�^�b�`����R���|�[�l���g.
/// �S�Ă̕���͌ŗL�̃p�����[�^������(���x�┭�ˊԊu��)
/// </summary>
class WeaponComponent :
    public Component
{
public:
    WeaponComponent(class Actor* owner);
    ~WeaponComponent();

    // �������e��h���N���X�Őݒ肷��,Lv1�ȍ~�̋������e��ݒ肷��
    // preLv��,Lv1->Lv2�Ȃ�,1�̕�,�܂背�x���A�b�v�O�̃��x��
    virtual void LevelUp(int preLv);
    virtual void Reset();

    void SetSizeFactor(float factor) { mSizeFactor = factor; }
    void SetIntervalFactor(float factor) { mIntervalFactor = factor; }
    void SetSpeedFactor(float factor) { mSpeedFactor = factor; }

protected:
    float mSizeFactor;
    float mIntervalFactor;
    float mSpeedFactor;
};

