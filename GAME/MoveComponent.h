#pragma once
#include "Component.h"

/// <summary>
/// �O�㍶�E�̈ړ��@�\��t�^����Component
/// </summary>
class MoveComponent :
    public Component
{
public:
    MoveComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;

    float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    float GetStrafeSpeed() const { return mStrafeSpeed; }
    
    void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
    void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }

private:
    float mAngularSpeed;
    float mForwardSpeed;
    float mStrafeSpeed; // strafe : �J�j����
};

