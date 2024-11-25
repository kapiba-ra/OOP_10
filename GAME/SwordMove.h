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
    // ��]�̒��S
    class Vector3 mPivot;
    // ��]���a�����߂����,��x�����ݒ肳��邱�Ƃ����҂���
    class Vector3 mOffset;
};

