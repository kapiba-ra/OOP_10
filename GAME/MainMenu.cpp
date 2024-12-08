#include "MainMenu.h"
#include "Game.h"
#include "Renderer.h"
#include "DialogBox.h"
#include <SDL/SDL.h>

/// <summary>
/// メインメニュー(スタート画面)のUI
/// </summary>
MainMenu::MainMenu(Game* game)
	: UIScreen(game)
{
	SetRelativeMouseMode(false);

	// 背景
	mBGPos = Vector2(0.0f, 0.0f);
	mBackground = mGame->GetRenderer()->GetTexture("Assets/MainMenuBG.png");
	// ボタン・背景
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2::Zero;

	SetTitle("MainTitle");
	AddButton("StartButton", [this]() {
		mGame->ChangeState(Game::EGameplay);
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
				mGame->ChangeState(Game::EQuit);
		});
	});
	AddButton("ControlsButton", [this]() {
		new DialogBox(mGame, "ControlsText",
			[this]() {
				mGame->ChangeState(Game::EMainMenu);
		});
	});
}

MainMenu::~MainMenu()
{
	SetRelativeMouseMode(true);
}
