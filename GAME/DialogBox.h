#pragma once
#include "UIScreen.h"

/// <summary>
/// ダイアログボックスのスクリーン
/// OKボタンとCancelボタンがある,OKを押した場合の処理はnewするときに設定する
/// </summary>
class DialogBox :
    public UIScreen
{
public:
    // 3つ目の引数は,cancelボタンを含むかどうか
    DialogBox(class Game* game, const std::string& text,
        std::function<void()> onOK,
        bool cancelButton = true);
    ~DialogBox();
};
