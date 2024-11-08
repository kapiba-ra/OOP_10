#include "LevelUpMenu.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "SkillSystem.h"
#include <SDL/SDL.h>

#include "PlayerActor.h"

LevelUpMenu::LevelUpMenu(Game* game)
	: UIScreen(game)
	, mSlideState(Slide::ESlideIn)
	, mSlideEndPosX(0.0f)
	, mSlideSpeed(2000.0f)
{
	mSkillSystem = mGame->GetSkillSystem();

	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOn.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOff.png");
	
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
	mNextButtonPos = Vector2(mSlideStartPosX, 50.0f);

	AddButtonRandom();
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
	// メニュー画面のタイトル(LevelUP!)
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
		DrawTexture(shader, b->GetNameTex(), b->GetPosition() + Vector2(50.0f, 0.0f), 0.5f);
		++i;
	}
}

void LevelUpMenu::AddButtonRandom()
{
	Vector2 buttonOffset = Vector2(0.0f, 10.0f);

	//std::unordered_map<PlayerActor::Skills, std::string> skillToButtonName = {
	//	{PlayerActor::Skills::s_shotNum, "IncBulletButton"},
	//	{PlayerActor::Skills::s_moveSpeed, "MoveSpeedUpButton"},
	//	{PlayerActor::Skills::s_ballScale, "BiggerBulletButton"}
	//};
	//
	//std::unordered_map<PlayerActor::Skills, int> Lvs = mGame->GetPlayer()->GetSkillLvs();
	//for (auto lv : Lvs)
	//{
	//	if (lv.second >= 8)
	//	{
	//		skillToButtonName[lv.first].erase();
	//	}
	//}
	//
	//// random
	//std::random_device rd;
	//std::mt19937 gen(rd());
	//std::uniform_int_distribution<> dist(0, skillToButtonName.size() - 1);
	//int randomIndex = dist(gen);
	//// random
	
	// パラメータ名を持つ必要がある
	// "Assets/" + ParamName + "BG.png"
	// ParamName + "Button"

	PlayerActor* player = mGame->GetPlayer();

	std::vector<Skill*> skills = mSkillSystem->GetRandomSkills();
	for (auto skill : skills)
	{
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/" + skill->name + "BG.png"));
		AddButton(skill->name, [this, skill, player]() {
			mGame->ChangeState(Game::EGameplay);
			skill->curLv += 1;
			player->OnLvUpSkill(skill->name);
			Close();
		}, buttonOffset);
	}
	if (skills.size() == 0)
	{
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/RecoverBG.png"));
		AddButton("Recover", [this, player]() {
			mGame->ChangeState(Game::EGameplay);
			player->OnLvUpSkill("Recover");
			Close();
		}, buttonOffset);
	}

	/* rekishi */
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/IncBulletBG.png"));
	//AddButton("IncBulletButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_shotNum);
	//	Close();
	//}, buttonOffset);
	//
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/MoveSpeedUpBG.png"));
	//AddButton("MoveSpeedUpButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_moveSpeed);
	//	Close();
	//}, buttonOffset);
	//
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/BiggerBulletBG.png"));
	//AddButton("BiggerBulletButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_ballScale);
	//	Close();
	//}, buttonOffset);
	//
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/MaxHpUpBG.png"));
	//AddButton("MaxHpUpButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_);
	//	Close();
	//}, buttonOffset);
	//
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/ShotSpeedUpBG.png"));
	//AddButton("ShotSpeedUpButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_);
	//	Close();
	//}, buttonOffset);
	//
	//mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/ShotIntervalUpBG.png"));
	//AddButton("ShotIntervalUpButton", [this]() {
	//	mGame->ChangeState(Game::EGameplay);
	//	mGame->GetPlayer()->LvUpSkill(PlayerActor::Skills::s_);
	//	Close();
	//}, buttonOffset);
}
