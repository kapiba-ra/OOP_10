#pragma once
#include "Component.h"

// Actor�����[�_�[�ɕ\�����邽�߂����ɕK�v

class TargetComponent :
    public Component
{
public:
    TargetComponent(class Actor* owner);
    ~TargetComponent();
};

