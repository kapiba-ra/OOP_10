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

    //  �������e��h���N���X�Őݒ肷��,Lv2�ȍ~�̋������e��ݒ肷��
    virtual void LevelUp(int CurLv);

    void SetSizeFactor(float factor) { mSizeFactor = factor; }
    void SetIntervalFactor(float factor) { mIntervalFactor = factor; }
    void SetSpeedFactor(float factor) { mSpeedFactor = factor; }

protected:
    float mSizeFactor;
    float mIntervalFactor;
    float mSpeedFactor;
};

