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

    void AddTargetComponent(class TargetComponent* tc);
    void RemoveTargetComponent(class TargetComponent* tc);

protected:
    // Update���ŌĂяo���w���p�[�֐�
    void UpdateCrosshair(float deltaTime);
    void UpdateRadar(float deltaTime);

    // �\�����̃e�N�X�`��
    class Texture* mCrosshair;
    class Texture* mCrosshairEnemy;
    // �\�������G�𑨂��Ă��邩
    bool mTargetEnemy;

    // HP�o�[�̃e�N�X�`��
    class Texture* mHPbar;

    // ���[�_�[�p�̃e�N�X�`��
    class Texture* mRader;
    class Texture* mBlipTex;
    // ���[�_�[���S����P�_�ւ�2D���΃I�t�Z�b�g
    std::vector<Vector2> mBlips;
    // ���[�_�[��(���[���h��Ԃł�)�͈͂�(���[�_�[�f�B�X�v���C��)���a
    float mRaderRange;
    float mRaderRadius;


    std::vector<class TargetComponent*> mTargetComps;
private:
    float mHPdiscardRange;
public:
    void SetHPdiscardRange(float range) { mHPdiscardRange = range; }
};

