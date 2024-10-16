#include "GameOverMenu.h"
#include "Game.h"
#include "Font.h"

GameOverMenu::GameOverMenu(Game* game)
	: UIScreen(game)
	, mContinueTextPos(0.0f, 0.0f)
{
	//mGame->SetState(Game::GameState::EGameover);
	//mGame->ChangeState(Game::EGameover);

	SetRelativeMouseMode(false);
	mTitlePos = Vector2(0.0f, 150.0f);
	SetTitle("GameOverTitle", Color::Black, 52);
	mContinueText = mFont->RenderText("ContinueText", Color::LightGreen, 46);

	mNextButtonPos = Vector2(0.0f, -100.0f);
	AddButton("YesButton", [this]() {
		Close();
		SetRelativeMouseMode(true);
		// リトライ
		//mGame->SetState(Game::GameState::EGameplay);
		//mGame->OnChangeState(Game::GameState::EGameplay);
		mGame->ChangeState(Game::EGameplay);
	});
	AddButton("NoButton", [this]() {
		Close();
		SetRelativeMouseMode(true);
		//mGame->SetState(Game::GameState::EMainMenu);
		//mGame->OnChangeState(Game::EMainMenu);
		mGame->ChangeState(Game::EMainMenu);
	});
}

GameOverMenu::~GameOverMenu()
{
}

void GameOverMenu::Draw(Shader* shader)
{
	UIScreen::Draw(shader);

	// Continueを促すメッセージを表示
	if (mContinueText)
	{
		DrawTexture(shader, mContinueText, mContinueTextPos);
	}
}
