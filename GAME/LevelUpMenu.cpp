#include "LevelUpMenu.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include <SDL/SDL.h>

#include "PlayerActor.h"

LevelUpMenu::LevelUpMenu(Game* game)
	: UIScreen(game)
	, mSlideState(Slide::ESlideIn)
	, mSlideEndPosX(0.0f)
	, mSlideSpeed(2000.0f)
{

	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOn.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOff.png");
	
	SetRelativeMouseMode(false);

	// Menuの背景
	mBackground = mGame->GetRenderer()->GetTexture("Assets/LevelUpMenuBG.png");
	// SlideがStartするx座標の設定
	float offset = mBackground->GetWidth() / 2.0f;
	float scLeftend = mGame->GetRenderer()->GetScreenWidth() / -2.0f;
	mSlideStartPosX = scLeftend - offset;
	mBGPos = Vector2(mSlideStartPosX, 0.0f);
	// 選択する各ボタンの背景
	mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/IncBulletBG.png"));
	mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/MoveSpeedUpBG.png"));

	mTitlePos = Vector2(mSlideStartPosX, 150.0f);
	mNextButtonPos = Vector2(mSlideStartPosX, 50.0f);

	SetTitle("LevelUpTitle");
	Vector2 buttonOffset = Vector2(0.0f, 10.0f);
	AddButton("IncBulletButton", [this]() {
		mGame->ChangeState(Game::EGameplay);
		mGame->GetPlayer()->IncShotNum();
		Close();
	}, buttonOffset);
	AddButton("MoveSpeedUpButton", [this]() {
		mGame->ChangeState(Game::EGameplay);
		// スピードアップの処理
		mGame->GetPlayer()->MoveSpeedUp();
		Close();
	}, buttonOffset);
}

LevelUpMenu::~LevelUpMenu()
{
	SetRelativeMouseMode(true);
}

void LevelUpMenu::Update(float deltaTime)
{
	// 動かしながら表示させる
	if (mSlideState == Slide::ESlideIn)
	{
		// 約0.33秒でUIが真ん中へ到達
		float moveAmount = mSlideSpeed * deltaTime;

		// 背景とタイトルの移動
		mBGPos.x += moveAmount;
		mTitlePos.x += moveAmount;
		// ボタン位置の移動
		for (auto b : mButtons)
		{
			b->SetPosition(b->GetPosition() + Vector2(moveAmount, 0.0f));
		}

		if (mBGPos.x >= -moveAmount + mSlideEndPosX) // FadeEndPosXは実際0だが一応
		{
			mBGPos.x = mSlideEndPosX;
			mTitlePos.x = mSlideEndPosX;
			for (auto b : mButtons)
			{
				Vector2 buttonPos = b->GetPosition();
				buttonPos.x = mSlideEndPosX;
				b->SetPosition(buttonPos);
			}
			mSlideState = Slide::EStop;
		}
	}
}

void LevelUpMenu::Draw(Shader* shader)
{
	// メニュー画面の背景
	if (mBackground)
	{
		DrawTexture(shader, mBackground, mBGPos);
	}
	// メニュー画面のタイトル
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	int i = 0;
	for (auto b : mButtons)
	{
		// ボタンの背景を描画
		if (mButtonBGs[i])
		{
			DrawTexture(shader, mButtonBGs[i], b->GetPosition());
		}
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		// ボタン(フレーム的な役割)とテキストを描画,テキストがボタンに依存してる感じ
		DrawTexture(shader, tex, b->GetPosition());
		DrawTexture(shader, b->GetNameTex(), b->GetPosition() + Vector2(50.0f, 0.0f));
		++i;
	}
}
