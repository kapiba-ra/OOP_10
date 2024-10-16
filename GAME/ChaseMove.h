#pragma once
#include "MoveComponent.h"
#include <vector>
#include "Math.h"
#include "AStar.h"

class ChaseMove :
    public MoveComponent
{
public:
    ChaseMove(class Actor* owner);

    void Update(float deltaTime) override;

    enum ChaseState   // �ǐՏ��
    {
        ETargeting, // �Ώۂ�F�����Ă���
        ESearching  // �ΏۂƂ̊Ԃɏ�Q��������
    };

    // �����Ŏw�肳�ꂽ�_�Ɍ����悤������ς���
    void TurnTo(const class Vector3& pos);

private:

    void CheckPath(float deltaTime);
    void OnEnter();

    ChaseState mChaseState;
    float mInterval;

    // �ǐՑΏۂ�PlayerActor�ɂ��Ă���,Actor�ɂ��ăL���X�g����悤�ɂ�����g�����͂��邪
    // ���ʂ͓G����Player�ւ̒ǐՂ����Ȃ����낤�Ƃ�������
    class PlayerActor* mPlayer;

    std::vector<Vector3> mPath;
    class Vector3 mNextPoint; // Searching�̎��Ɏ��Ɍ������_
};

