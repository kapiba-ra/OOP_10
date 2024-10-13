#include "GameOverMenu.h"
#include "Game.h"


GameOverMenu::GameOverMenu(Game* game)
	: UIScreen(game)
{
	mGame->SetState(Game::GameState::EGameover);

	SetRelativeMouseMode(false);
	SetTitle("GameOverTitle");
	AddButton("ContinueButton", [this]() {
		Close();
		SetRelativeMouseMode(true);
		mGame->SetState(Game::GameState::EMainMenu)
	});
	AddButton("ToMainMenuButton", [this]() {
		Close();
		SetRelativeMouseMode(true);
		mGame->SetState(Game::GameState::EMainMenu)
	});
}

GameOverMenu::~GameOverMenu()
{
}
