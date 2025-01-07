#pragma once
#include "UIScreen.h"

/// <summary>
/// �_�C�A���O�{�b�N�X�̃X�N���[��
/// OK�{�^����Cancel�{�^��������,OK���������ꍇ�̏�����new����Ƃ��ɐݒ肷��
/// </summary>
class DialogBox :
    public UIScreen
{
public:
    // 3�ڂ̈�����,cancel�{�^�����܂ނ��ǂ���
    DialogBox(class Game* game, const std::string& text,
        std::function<void()> onOK,
        bool cancelButton = true);
    ~DialogBox();
};
