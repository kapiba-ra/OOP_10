#include "PauseMenu.h"
#include "Game.h"
#include "DialogBox.h"
#include "MainMenu.h"
#include <SDL/SDL.h>

PauseMenu::PauseMenu(Game* game)
	: UIScreen(game)
	, mDialogBox(nullptr)
{
	//mGame->SetState(Game::EPaused);
	SetRelativeMouseMode(false);
	SetTitle("PauseTitle");
	AddButton("ResumeButton", [this]() {
		Close();
		SetRelativeMouseMode(true);
		//mGame->SetState(Game::EGameplay);
		mGame->ChangeState(Game::EGameplay);
	});
	AddButton("QuitButton", [this]() {
		mDialogBox = new DialogBox(mGame, "QuitText",
			[this]() {
				this->Close();		 // PauseMenu��Close()
				mDialogBox->Close(); // DialogBox��Close()
				//mGame->OnChangeState(Game::EMainMenu, Game::EPaused);
				mGame->ChangeState(Game::EMainMenu);
		});
	});
}

PauseMenu::~PauseMenu()
{
	SetRelativeMouseMode(true); 
}

void PauseMenu::HandleKeyPress(int key)
{
	UIScreen::HandleKeyPress(key);
	
	if (key == SDLK_ESCAPE)
	{
		Close();
		//mGame->SetState(Game::EGameplay);
		mGame->ChangeState(Game::EGameplay);
	}
}
