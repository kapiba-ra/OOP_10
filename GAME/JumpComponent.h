#pragma once
#include "Component.h"

/// <summary>
/// �W�����v�@�\��t�^����Component
/// MoveComponent�ƕ������݌v�Ӑ}�́A2�i�W�����v��d�͕ω��ŋ@�\�����G�����邱�Ƃ�z�肵��
/// Land�𐳂����Ăяo���Ȃ��Ɩ����ɗ������x������������̂Œ���
/// </summary>
class JumpComponent :
    public Component
{
public:
    JumpComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;

    // �W�����v�̓��؂�ŌĂяo��
    void Liftoff(float speed);
    // ���n����Ƃ��ɌĂяo��
    void Land();
    bool IsJumping() const { return mIsJumping; }
    
    float GetJumpSpeed() const { return mJumpSpeed; }
    void SetJumpSpeed(float speed) { mJumpSpeed = speed; }

private:
    bool mIsJumping;
    float mJumpSpeed;
    float mGravity;
};