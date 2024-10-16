#pragma once
#include "Actor.h"

// �A�C�e���̎�ނ�����������N���X�ɂ���������

class ItemActor :
    public Actor
{
public:
    ItemActor(class Game* game);

    void Reset() override;

private:
    class MoveComponent* mMoveComp;
    class BoxComponent* mBoxComp;
};

