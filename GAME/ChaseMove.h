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
    // �����ƖڕW�̊Ԃɂ����Q���𒲂ׂ�
    void CheckObstacle(float deltaTime);
    void OnEnter();

    ChaseState mChaseState;
    float mInterval;        // �o�H�Čv�Z�܂ł̎���

    // �ǐՑΏۂ�PlayerActor�ɂ��Ă���,Actor�ɂ��ăL���X�g����悤�ɂ�����g�����͂��邪
    // ���ʂ͓G����Player�ւ̒ǐՂ����Ȃ����낤�Ƃ�������
    class PlayerActor* mPlayer;

    std::vector<Vector3> mPath; // �o�H�T���ɂ��ړ�������Ƃ���,Player�܂ł̌o�H��ۑ�����
    class Vector3 mNextPoint;   // Searching�̎��Ɏ��Ɍ������_
};

