#include "LevelUpMenu.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "SkillSystem.h"
#include <SDL/SDL.h>

#include "UIAnimation.h"

#include "PlayerActor.h"
#include "HpComponent.h"

LevelUpMenu::LevelUpMenu(Game* game)
	: UIScreen(game)
	, mSlideState(Slide::ESlideIn)
	, mSlideEndPosX(0.0f)
	, mSlideSpeed(2000.0f)
{
	// ボタンのテクスチャを設定,Offは透明,Onはピンクの枠
	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOn.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOff.png");
	
	// Menuに表示するテキストを設定
	SetTitle("LevelUpTitle");
	
	SetRelativeMouseMode(false);

	// Menuの背景
	mBackground = mGame->GetRenderer()->GetTexture("Assets/LevelUpMenuBG.png");
	// SlideがStartするx座標の設定
	float offset = mBackground->GetWidth() / 2.0f;
	float scLeftend = mGame->GetRenderer()->GetScreenWidth() / -2.0f;
	mSlideStartPosX = scLeftend - offset;
	
	mBGPos = Vector2(mSlideStartPosX, 0.0f);
	mTitlePos = Vector2(mSlideStartPosX, 150.0f);
	mBGSlider = new Slider(mSlideEndPosX, mSlideSpeed, mBGPos);
	mTitleSlider = new Slider(mSlideEndPosX, mSlideSpeed, mTitlePos);
	mNextButtonPos = Vector2(mSlideStartPosX, 50.0f);
	
	AddButtonRandom();
	
	for (auto b : mButtons)
	{
		Slider* s = new Slider(mSlideEndPosX, mSlideSpeed, b->GetPosition());
		mButtonSliders.push_back(s);
	}

}

LevelUpMenu::~LevelUpMenu()
{
	delete mTitleSlider;
	mTitleSlider = nullptr;
	delete mBGSlider;
	mBGSlider = nullptr;
	while(!mButtonSliders.empty())
	{
		delete mButtonSliders.back();
		mButtonSliders.pop_back();
	}

	if (mGame->GetState() != Game::ELevelUp)
	{
		SetRelativeMouseMode(true);
	}
}

void LevelUpMenu::Update(float deltaTime)
{
	// 位置のUpdateを行うが、Sliderクラスに任せている
	mTitleSlider->Update(deltaTime);
	mTitlePos = mTitleSlider->GetPosition();
	mBGSlider->Update(deltaTime);
	mBGPos = mBGSlider->GetPosition();
	for (size_t i = 0; i < mButtons.size(); ++i)
	{
		if (i < mButtonSliders.size())
		{
			mButtonSliders[i]->Update(deltaTime);
			mButtons[i]->SetPosition(mButtonSliders[i]->GetPosition());
		}
	}

	// 動かしながら表示させる
	//if (mSlideState == Slide::ESlideIn)
	//{
	//	// 約0.33秒でUIが真ん中へ到達
	//	float moveAmount = mSlideSpeed * deltaTime;
	//
	//	// 背景とタイトルの移動
	//	mBGPos.x += moveAmount;
	//	mTitlePos.x += moveAmount;
	//	// ボタン位置の移動
	//	for (auto b : mButtons)
	//	{
	//		b->SetPosition(b->GetPosition() + Vector2(moveAmount, 0.0f));
	//	}
	//
	//	if (mBGPos.x >= -moveAmount + mSlideEndPosX) // FadeEndPosXは実際0だが一応
	//	{
	//		mBGPos.x = mSlideEndPosX;
	//		//mTitlePos.x = mSlideEndPosX;
	//		for (auto b : mButtons)
	//		{
	//			Vector2 buttonPos = b->GetPosition();
	//			buttonPos.x = mSlideEndPosX;
	//			b->SetPosition(buttonPos);
	//		}
	//		mSlideState = Slide::EStop;
	//	}
	//}
}

void LevelUpMenu::Draw(Shader* shader)
{
	// メニュー画面の背景
	if (mBackground)
	{
		DrawTexture(shader, mBackground, mBGPos);
	}
	// メニュー画面のタイトル(LevelUP!)
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	int i = 0;
	for (auto b : mButtons)
	{
		// ボタンのアイコンを描画
		if (mButtonBGs[i] && mIcons[i])
		{
			DrawTexture(shader, mButtonBGs[i], b->GetPosition());
			DrawTexture(shader, mIcons[i], b->GetPosition() - Vector2(165.0f, 0.0f));
		}
		++i;
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		// ボタン(ピンクの枠みたいな,カーソル乗ってないと何もない)とテキストを描画
		// テキストとボタンはセットになっている
		DrawTexture(shader, tex, b->GetPosition());
		DrawTexture(shader, b->GetNameTex(), b->GetPosition() + Vector2(50.0f, 0.0f), 0.5f);
	}
}

void LevelUpMenu::AddButtonRandom()
{
	Vector2 buttonOffset = Vector2(0.0f, 10.0f);

	PlayerActor* player = mGame->GetPlayer();
	SkillSystem* skillSystem = mGame->GetSkillSystem();
	
	std::vector<Skill*> skills = skillSystem->GetRandomSkills();
	for (auto skill : skills)
	{
		// BGの部分は変わるかも
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/SkillBG.png"));
		mIcons.emplace_back(skill->GetIconTex());
		AddButton(skill->GetName() + std::to_string(skill->GetCurLv()), [this, skill, skillSystem]() {
			mGame->ChangeState(Game::EGameplay);
			skillSystem->OnLevelUp(skill);
			Close();
		}, buttonOffset);
	}
	// 全部の武器とパークのレベルがmaxになったら,回復だけできるようになる
	if (skills.size() == 0)
	{
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/SkillBG.png"));
		mIcons.emplace_back(skillSystem->GetRecoverIcon());
		AddButton("Recover", [this, player]() {
			mGame->ChangeState(Game::EGameplay);
			player->GetHpComp()->Recover(20.0f);
			Close();
		}, buttonOffset);
	}
}
