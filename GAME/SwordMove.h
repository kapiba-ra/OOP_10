#pragma once
#include "MoveComponent.h"
#include "Math.h"

/// <summary>
/// SwordWeaponの挙動用のComponent,中心(Pivot)を通るZ方向軸で回転する
/// 実際はPivotMoveのような名前で運用してもいいかも？(というかそれを目指した)
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
    // 回転の中心
    class Vector3 mPivot;
    // 回転の中心からどれくらい離れるか
    class Vector3 mOffset;
};

