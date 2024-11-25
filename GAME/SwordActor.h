#pragma once
#include "WeaponActor.h"

/// <summary>
/// ����:�\�[�h
/// SwordMove���쐬����K�v������
/// </summary>
class SwordActor :
    public WeaponActor
{
public:
    SwordActor(class Game* game);

    void UpdateActor(float deltaTime) override;

    // �G�Ƃ̏Փ�
    void FixCollision();

    void SetShotSpeed(float speed);
    void SetRotationSpeed(float speed);
    
    // ��]�̒��S�Ɗ�ƂȂ�Offset(��]���a)��ݒ肷��֐�
    void SetPivotAndRadius(class Actor* actor, const class Vector3& offset);

private:
    // ��]���S�ƂȂ�A�N�^�[
    // ���̃|�C���^��SwordActor���ɗp�ӂ��鎖�ŁASwordMove��
    // "�^����ꂽ���W�𒆐S�ɉ�]����"�Ƃ����@�\�ɗ��߂�
    class Actor* mPivotActor;

    class SwordMove* mMyMove;
    class BoxComponent* mBoxComp;
    //class AudioComponent* mAudioComp;
};
