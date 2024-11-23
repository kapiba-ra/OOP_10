#pragma once
#include "Component.h"

/// <summary>
/// �A�N�^�[��Hp����������.
/// Component�������̂́A�G�E�v���C���[�E�j��ł���I�u�W�F�N�g���ɃA�^�b�`���邱�Ƃ�z�肵������
/// </summary>
class HpComponent :
    public Component
{
public:
    HpComponent(class Actor* owner, float maxHp = 1.0f);
    
    void Update(float deltaTime) override;
    void Reset(float initialHp);

    void TakeDamage(float damage);
    void Recover(float recover);

    // 0�ȉ���
    bool IsKilled() const { return mCurHp <= 0.0f; }

    // Getter,Setter
    float GetCurHp() const { return mCurHp; }
    void SetMaxHp(float maxHp) { mMaxHp = maxHp; }
    // �֗��n(SetterGetter�ł��ł��邯��...�Ƃ����z)
    float GetHpPercentage() const { return mCurHp / mMaxHp; }
    void AddMaxHp(float add);

private:
    float mCurHp;
    float mMaxHp;
};

