#pragma once
#include "Actor.h"

/// <summary>
/// ����̊��N���X
/// </summary>
class Weapon :
    public Actor
{
public:
    Weapon(class Game* game);
    virtual ~Weapon();

    void Reset() override;

// ����
protected:
    int mCurLevel;
    int mMaxLevel;
};

