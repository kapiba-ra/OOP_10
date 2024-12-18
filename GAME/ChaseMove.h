#pragma once
#include "MoveComponent.h"
#include <vector>
#include "Math.h"
#include "AStar.h"

class ChaseMove :
    public MoveComponent
{
public:
    ChaseMove(class Actor* owner, class Actor* target);

    void Update(float deltaTime) override;

    enum ChaseState   // 追跡状態
    {
        ETargeting, // 対象を認識している
        ESearching  // 対象との間に障害物がある
    };

    // 引数で指定された点に向くよう方向を変える
    void TurnTo(const class Vector3& pos);

    void SetTarget(class Actor* target);

private:
    // 自分と目標の間にある障害物を調べる
    void CheckObstacle(float deltaTime);
    void OnEnter();

    ChaseState mChaseState;
    float mInterval;        // 経路再計算までの時間

    class Actor* mTarget;   // Chase対象

    std::vector<Vector3> mPath; // 経路探索による移動をするときに,Playerまでの経路を保存する
    class Vector3 mNextPoint;   // Searchingの時に次に向かう点
};

