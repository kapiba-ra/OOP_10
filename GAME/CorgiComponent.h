#pragma once
#include "WeaponComponent.h"

/// <summary>
/// �R�[�M�[�����p�̃R���|�[�l���g
/// ��������A�N�^�[�͉i����1�Ȃ̂�,�|�C���^����������
/// </summary>
class CorgiComponent :
    public WeaponComponent
{
public:
    CorgiComponent(class Actor* owner);

    void LevelUp(int preLv) override;
    void Reset() override;
    float GetIntervalRate() override { return 1.0f; }
private:
    class CorgiActor* mCorgi;

    bool mTargetActive; // �^�[�Q�b�g�������Ă��邩

    float mScale;
    float mSpeed;
};

