#pragma once
#include "MoveComponent.h"
#include "Math.h"

class SwordMove :
    public MoveComponent
{
public:
    SwordMove(class Actor* owner);

    void Update(float deltaTime) override;

    void SetPivot(const class Vector3& pivot) { mPivot = pivot; }
    void SetOffset(const class Vector3& offset) { mOffset = offset; }

private:
    // ‰ñ“]‚Ì’†S
    class Vector3 mPivot;
    // ‰ñ“]”¼Œa‚ğŒˆ‚ß‚é‚à‚Ì,ˆê“x‚¾‚¯İ’è‚³‚ê‚é‚±‚Æ‚ğŠú‘Ò‚·‚é
    class Vector3 mOffset;
};

