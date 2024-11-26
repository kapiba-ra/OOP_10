#pragma once
#include "MoveComponent.h"
#include "Math.h"

/// <summary>
/// SwordWeapon�̋����p��Component,���S(Pivot)��ʂ�Z�������ŉ�]����
/// ���ۂ�PivotMove�̂悤�Ȗ��O�ŉ^�p���Ă����������H(�Ƃ����������ڎw����)
/// </summary>
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
    // ��]�̒��S����ǂꂭ�炢����邩
    class Vector3 mOffset;
};

