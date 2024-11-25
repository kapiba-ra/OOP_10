#pragma once
#include "WeaponActor.h"

/// <summary>
/// 武器:ソード
/// SwordMoveを作成する必要がある
/// </summary>
class SwordActor :
    public WeaponActor
{
public:
    SwordActor(class Game* game);

    void UpdateActor(float deltaTime) override;

    // 敵との衝突
    void FixCollision();

    void SetShotSpeed(float speed);
    void SetRotationSpeed(float speed);
    
    // 回転の中心と基準となるOffset(回転半径)を設定する関数
    void SetPivotAndRadius(class Actor* actor, const class Vector3& offset);

private:
    // 回転中心となるアクター
    // このポインタをSwordActor側に用意する事で、SwordMoveは
    // "与えられた座標を中心に回転する"という機能に留めた
    class Actor* mPivotActor;

    class SwordMove* mMyMove;
    class BoxComponent* mBoxComp;
    //class AudioComponent* mAudioComp;
};
