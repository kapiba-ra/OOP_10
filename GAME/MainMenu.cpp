#include "MainMenu.h"
#include "Game.h"
#include "Renderer.h"
#include "DialogBox.h"
#include <SDL/SDL.h>

MainMenu::MainMenu(Game* game)
	: UIScreen(game)
{
	// mGame->SetState(Game::EMainMenu);
	SetRelativeMouseMode(false);

	// ”wŒi
	mBGPos = Vector2(0.0f, 0.0f);
	mBackground = mGame->GetRenderer()->GetTexture("Assets/MainMenuBG.png");
	// ƒ{ƒ^ƒ“E”wŒi
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2::Zero;

	SetTitle("MainTitle");
	AddButton("StartButton", [this]() {
		//mGame->SetState(Game::EGameplay);
		//mGame->OnChangeState(Game::EGameplay);
		mGame->ChangeState(Game::EGameplay);
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
				//mGame->SetState(Game::EQuit);
				mGame->ChangeState(Game::EQuit);
		});
	});
}

MainMenu::~MainMenu()
{
	SetRelativeMouseMode(true);
}
