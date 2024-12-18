#pragma once
#include "WeaponComponent.h"

class CorgiComponent :
    public WeaponComponent
{
public:
    CorgiComponent(class Actor* owner);

    void LevelUp(int curLv) override;
    void Reset() override;

private:

    class CorgiActor* mCorgi;

    bool mTargetActive; // �^�[�Q�b�g�������Ă��邩

    float mScale;
    float mSpeed;
};

