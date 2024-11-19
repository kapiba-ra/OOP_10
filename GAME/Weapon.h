#pragma once
#include "Actor.h"

/// <summary>
/// •Ší‚ÌŠî’êƒNƒ‰ƒX
/// </summary>
class Weapon :
    public Actor
{
public:
    Weapon(class Game* game);
    virtual ~Weapon();

    void Reset() override;

// ”à
protected:
    int mCurLevel;
    int mMaxLevel;
};

