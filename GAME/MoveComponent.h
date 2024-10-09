#pragma once
#include "Component.h"

class MoveComponent :
    public Component
{
public:
    MoveComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;

    float GetAngularSpeed() const { return mAngularSpeed; }
    float GetForwardSpeed() const { return mForwardSpeed; }
    float GetStrafeSpeed() const { return mStrafeSpeed; }
    //float GetJumpSpeed() const { return mJumpSpeed; }
    
    void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
    void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetStrafeSpeed(float speed) { mStrafeSpeed = speed; }
    //void SetJumpSpeed(float speed) { mJumpSpeed = speed; }

private:
    float mAngularSpeed;
    float mForwardSpeed;
    float mStrafeSpeed; // strafe : ƒJƒj•à‚«
    //float mJumpSpeed;
};

