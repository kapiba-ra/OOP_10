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
    
    //  �������e��h���N���X�Őݒ肷��
    virtual void OnLevelUp();

protected:
    int mCurLevel;
    int mMaxLevel;

    // ����ŗL�̃p�����[�^�B
    int mUniqueNum;
    float mUniqueSize;
    float mUniqueInterval;
    float mUniqueSpeed;
};

// ���x���ɂ���ċ������e��ς�����悤�ɂ��ׂ�