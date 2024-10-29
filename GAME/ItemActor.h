#pragma once
#include "Actor.h"

// �A�C�e���̎�ނ�����������N���X�ɂ���������

class ItemActor :
    public Actor
{
public:
    // �g�����킩��Ȃ�����
    enum ItemType
    {
        Eexp,
    };

    ItemActor(class Game* game);
    ~ItemActor();
    
    void OnAcquired();
    void Reset() override;

private:

    class MoveComponent* mMoveComp;
    class BoxComponent* mBoxComp;
};

