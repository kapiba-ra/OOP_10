#pragma once
#include "Actor.h"

/// <summary>
/// 武器の基底クラス
/// </summary>
class Weapon :
    public Actor
{
public:
    Weapon(class Game* game);
    virtual ~Weapon();

    void Reset() override;
    
    //  強化内容を派生クラスで設定する
    virtual void OnLevelUp();

protected:
    int mCurLevel;
    int mMaxLevel;

    // 武器固有のパラメータ達
    int mUniqueNum;
    float mUniqueSize;
    float mUniqueInterval;
    float mUniqueSpeed;
};

// レベルによって強化内容を変えられるようにすべき