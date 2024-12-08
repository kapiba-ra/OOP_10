#pragma once
#include "Component.h"

/// <summary>
/// ジャンプ機能を付与するComponent
/// MoveComponentと分けた設計意図は、2段ジャンプや重力変化で機能が複雑化することを想定して
/// Landを正しく呼び出さないと無限に落下速度が増え続けるので注意
/// </summary>
class JumpComponent :
    public Component
{
public:
    JumpComponent(class Actor* owner, int updateOrder = 10);
    void Update(float deltaTime) override;

    // ジャンプの踏切りで呼び出す
    void Liftoff(float speed);
    // 着地するときに呼び出す
    void Land();
    bool IsJumping() const { return mIsJumping; }
    
    float GetJumpSpeed() const { return mJumpSpeed; }
    void SetJumpSpeed(float speed) { mJumpSpeed = speed; }

private:
    bool mIsJumping;
    float mJumpSpeed;
    float mGravity;
};