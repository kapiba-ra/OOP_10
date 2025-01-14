#pragma once

#include "Math.h"

/// <summary>
/// UI�̃A�j���[�V�����Ɋ֘A����v�Z������N���X.
/// ���܂芈�p�ł��Ă��Ȃ�(�t�F�[�h�A�E�g�ƃY�[���A�E�g���ȒP�ɂł����������)�B
/// </summary>
class UIAnimation
{
public:
    enum class AnimState
    {
        EPlaying,   // �A�j���[�V�����i�s��
        EEnded      // �A�j���[�V�����I��
    };

    UIAnimation(AnimState animState);
    virtual ~UIAnimation();

    virtual void Update(float deltaTime) = 0; // �X�V����

    void SetState(AnimState state) { mAnimState = state; }
    AnimState GetState() const { return mAnimState; }

protected:
    AnimState mAnimState;
};

/// <summary>
/// �X���C�h�C���A�E�g�̃��W�b�N��S��,�R���X�g���N�^�Ŋ֘A����p�����[�^��ݒ肷��
/// (���X�̓X���C�h������W�̎Q�Ƃ��󂯎���Ă�����,�ˑ��֌W���o�č������̂�GetPosition��
/// �s�x�Ԃ��悤�ɂ���)
/// �ǋL:Unity��Slider�Ƃ͈قȂ�@�\
/// </summary>
class Slider :
    public UIAnimation
{
public:
    Slider(float endX, float speed, Vector2 initialPos, 
        UIAnimation::AnimState animState = AnimState::EPlaying);
    ~Slider();

    // ���̃N���X��Update�ƈႢ�AGame�N���X�ŏ���ɌĂ΂�Ȃ��̂�
    // �����ŌĂяo���K�v�����鎖�ɒ��ӁB
    void Update(float deltaTime) override;
    const Vector2& GetPosition() const { return mCurrentPos; }

private:
    float mEndX;            // �X���C�h�I���ʒu
    float mSpeed;           // �X���C�h���x
    Vector2 mCurrentPos;    // �ύX����ʒu�̌��݈ʒu
};