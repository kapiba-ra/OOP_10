#pragma once
#include "UIScreen.h"
#include <vector>

// HUD(�w�b�h�A�b�v�f�B�X�v���C)
// �R���s���[�^�[�Q�[�����ŉ�ʂɏd�˂ĕ\���������̂���

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

    // HP�o�[�̃e�N�X�`��
    class Texture* mHPbar;
    class Texture* mHPbarBG;
    float mHPdiscardRange;

    // ���[�_�[�p�̃e�N�X�`��
    class Texture* mRader;
    class Texture* mBlipTex;
    // ���[�_�[���S����P�_�ւ�2D���΃I�t�Z�b�g
    std::vector<Vector2> mBlips;
    // ���[�_�[��(���[���h��Ԃł�)�͈͂ƃ��[�_�[�f�B�X�v���C�̔��a
    float mRaderRange;
    float mRaderRadius;

    // �^�C�}�[�p
    float mTimeFloat;

    // Level
    class Texture* mLevel;

    // Phase
    class Texture* mPhase;
    int mCurPhaseNum;

    std::vector<class TargetComponent*> mTargetComps;
};

