#pragma once
#include "UIScreen.h"
#include <vector> 

/// <summary>
/// HUD(�w�b�h�A�b�v�f�B�X�v���C),�Q�[�����ŉ�ʂɏd�˂ĕ\���������̂���
/// HUD�̕\����S������N���X
/// </summary>
class HUD :
    public UIScreen
{
public:
    HUD(class Game* game);
    ~HUD();

    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;
    void Reset() override;
    void ResetTimer();

    void AddTargetComponent(class TargetComponent* tc);
    void RemoveTargetComponent(class TargetComponent* tc);

protected:
    // Update���ŌĂяo���w���p�[�֐�
    void UpdateRadar(float deltaTime);
    void UpdateTimer(float deltaTime);

    /* HP */
    class Texture* mHPbarGreen; // �΂�HPbar
    class Texture* mHPbarRed;   // �Ԃ�HPbar
    class Texture* mHPbarBG;    // HPbar�̔w�i
    float mHPdiscardRange;      // Hpbar�̕`��͈͂�����(0-1)
    /* ���[�_�[ */
    class Texture* mRadar;          // ���[�_�[Display�̊ۂ��~
    class Texture* mPlayerInRadar;  // ���[�_�[���S�̃v���C���[
    class Texture* mBlipTex;        // �G�̈ʒu������
    std::vector<Vector2> mBlips;    // ���[�_�[���S����P�_�ւ�2D���΃I�t�Z�b�g
    float mRadarRange;      // ���[���h��Ԃł̃��[�_�[�͈�
    float mRadarRadius;     // RaderDisplay�ł̃��[�_�[���a
    std::vector<class TargetComponent*> mTargetComps;   // �G�̈ʒu���擾�ł���
    /* �X�L���̃��x�� */
    std::vector<class Texture*> mIcons; // �l�������X�L���̃A�C�R��
    class Texture* mIconFrame;       // �A�C�R���̃t���[��

    class Texture* mLevel;  // �uLv.�v �̃e�L�X�g�e�N�X�`��
    class Texture* mPhase;  // �uPhase�v�̃e�L�X�g�e�N�X�`��
    int mCurPhaseNum;       // ���݂̃t�F�[�Y
    float mTimeFloat;       // Phase�J�n����̎��Ԃ��L�^����

    /* �\���ʒu�n,Update�Ŏg�����ɔ����ă����o�ɂ��Ă�����(��{��Draw�Ŏg�p) */
    class Vector2 mHpbarPos;    // Hpbar�̕\���ʒu
    class Vector2 mHpNumPos;    // Hp(����)�̕\���ʒu
    class Vector2 mRadarPos;    // Radar�̕\���ʒu
    class Vector2 mSkillPos;    // Skill�̕\���ʒu
    class Vector2 mLevelPos;    // �uLv.�v�\���ʒu
    class Vector2 mPhasePos;    // �uPhase�v�\���ʒu
    class Vector2 mTimePos;     // Phase�o�ߎ��Ԃ̃e�N�X�`���ʒu
};

