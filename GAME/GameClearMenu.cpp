#include "GameClearMenu.h"
#include "Game.h"
#include "Font.h"

GameClearMenu::GameClearMenu(Game* game)
	: UIScreen(game)
	, mCommentPos(0.0f, 0.0f)
{
	SetRelativeMouseMode(false);
	
	mTitlePos = Vector2(0.0f, 150.0f);
	SetTitle("GameClearTitle", Color::Black, FontSize::font_60);
	
	mComment = mFont->RenderText("ThankYouText", Color::Green, FontSize::font_42 );
	mCommentPos = Vector2(0.0f, -192.0f);

	mNextButtonPos = Vector2(0.0f, -300.0f);
	AddButton("OKButton", [this]() {
		Close();
		mGame->ChangeState(Game::EMainMenu);
	});
}

GameClearMenu::~GameClearMenu()
{
}

void GameClearMenu::Draw(Shader* shader)
{
	UIScreen::Draw(shader);

	if (mComment)
	{
		DrawTexture(shader, mComment, mCommentPos);
	}
}
