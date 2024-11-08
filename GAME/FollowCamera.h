#pragma once
#include "CameraComponent.h"

/*
*  �O�l�̎��_�̒Ǐ]�J�����B
*  �΂˂𗘗p���邱�ƂŁA���炩�ɒǂ������邱�Ƃ��ł���
*/

class FollowCamera :
    public CameraComponent
{
public:
    FollowCamera(class Actor* owner);

    void Update(float deltaTime) override;

    void SnapToIdeal();

    void SetHorzDist(float dist) { mHorzDist = dist; }
    void SetVertDist(float dist) { mVertDist = dist; }
    void SetTargetDist(float dist) { mTargetDist = dist; }
    void SetSpringConstant(float spring) { mSpringConstant = spring; }

    enum CameraState
    {
        EBack,
        EAbove
    };
    void SwitchCameraPos();
    

private:
    CameraState mCameraState;

    Vector3 ComputeCameraPos() const;

    Vector3 mActualPos; // �J�����̎��ۂ̈ʒu
    Vector3 mVelocity;  // ���ۂ̃J�����̑��x

    float mHorzDist;       // ��������
    float mVertDist;       // ��������
    float mTargetDist;     // �^�[�Q�b�g����
    float mSpringConstant; // �΂˒萔(�΂˂̍d����\��)
};

