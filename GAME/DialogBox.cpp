#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include <SDL/SDL.h>

DialogBox::DialogBox(Game* game, const std::string& text, std::function<void()> onOK)
	: UIScreen(game)
{
	// ダイアログボックス用に位置を調節する
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);
	// 背景のテクスチャを設定
	mBackground = mGame->GetRenderer()->GetTexture("Assets/DialogBG.png");
	SetTitle(text, Vector3::Zero, 30);
	// ボタンの設定
	AddButton("OKButton", [onOK]() {
		onOK();
	});
	AddButton("CancelButton", [this]() {
		Close();
	});
}

DialogBox::~DialogBox()
{
}
