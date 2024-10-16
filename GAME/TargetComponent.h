#pragma once
#include "Component.h"

// Actorをレーダーに表示するためだけに必要

class TargetComponent :
    public Component
{
public:
    TargetComponent(class Actor* owner);
    ~TargetComponent();
};

