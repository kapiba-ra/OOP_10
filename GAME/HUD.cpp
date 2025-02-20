#include "HUD.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "Texture.h"
#include "Font.h"
#include "PhaseSystem.h"
#include "SkillSystem.h"
#include "UIAnimation.h"

#include "Actor.h"
#include "PlayerActor.h"
#include "EnemyActor.h"

#include "TargetComponent.h"
#include "HpComponent.h"

HUD::HUD(Game* game)
	: UIScreen(game)
	, mRadarRange(2000.0f)
	, mRadarRadius(92.0f)
	, mHPdiscardRange(1.0f)
	, mRadarPos(-390.0f, 275.0f)
	, mHpbarPos(-350.0f, -350.0f)
	, mHpNumPos(-460.0f, -315.0f)
	, mTimePos(-20.0f, 340.0f)
	, mLevelPos(400.0f, 350.0f)
	, mPhasePos(300.0f, 350.0f)
	, mSkillPos(-480.0f, 0.0f)
{
	Renderer* r = mGame->GetRenderer();
	mRadar = r->GetTexture("Assets/Radar.png");
	mPlayerInRadar = r->GetTexture("Assets/RadarArrow.png");
	mBlipTex = r->GetTexture("Assets/Blip.png");
	mHPbarBG = r->GetTexture("Assets/HPBarBG.png");
	mHPbarGreen = r->GetTexture("Assets/HPBarGreen.png");
	mHPbarRed = r->GetTexture("Assets/HPBarRed.png");
	mIconFrame = r->GetTexture("Assets/Icon/Frame.png");
	mIconMask = r->GetTexture("Assets/Icon/Mask.png");
	mLevel = mFont->RenderText("LevelText");
	mPhase = mFont->RenderText("PhaseText");
	mExcellent = mFont->RenderText("ExcellentText", Color::Red, FontSize::font_60);
	mNextPhase = mFont->RenderText("NextPhaseText", Color::LightPink, FontSize::font_60);
}

HUD::~HUD()
{
	if (mSlider)
	{
		delete mSlider;
		mSlider = nullptr;
	}
}

void HUD::Update(float deltaTime)
{
	UIScreen::Update(deltaTime);
	UpdateRadar(deltaTime);
	UpdateTimer(deltaTime);
	if (mSlider && mGame->GetState() == Game::EGameplay)
	{
		mSlider->Update(deltaTime);
	}
}

void HUD::Draw(Shader* shader)
{
	// 準備。キャッシュしておくと表現する
	PlayerActor* player = mGame->GetPlayer();
	PlayerActor::Parameters pParams = player->GetParams();
	Vector2 offset(Vector2::Zero);	// 使いまわす
	PhaseSystem* phaseSystem = mGame->GetPhaseSystem();

	// レーダーの描画
	DrawTexture(shader, mRadar, mRadarPos);
	// レーダー上の輝点の描画
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, mRadarPos + blip);
	}
	DrawTexture(shader, mPlayerInRadar, mRadarPos);

	// HPバーの描画
	DrawTexture(shader, mHPbarBG, mHpbarPos);
	mHPdiscardRange = player->GetHpComp()->GetHpPercentage();
	if (mHPdiscardRange >= 0.2f)
	{
		DrawTexture(shader, mHPbarGreen, mHpbarPos, 1.0f, mHPdiscardRange);
	}
	else if (mHPdiscardRange >= 0.0f)
	{
		DrawTexture(shader, mHPbarRed, mHpbarPos, 1.0f, mHPdiscardRange);
	}
	// HP(number)の描画
	offset = Vector2(mNumbers[0]->GetWidthF(), 0.0f);
	std::string hp = std::to_string(static_cast<int>(player->GetHpComp()->GetCurHp()));
	for (size_t i = 0;i < hp.length(); i++)
	{
		int digitIndex = hp[i] - '0';
		if (0 <= digitIndex && digitIndex <= 9) //一応範囲外アクセスしないように
		{
			DrawTexture(shader, mNumbers[digitIndex], mHpNumPos + offset * static_cast<float>(i));
		}
	}

	// スキルのアイコンの描画
	Vector2 nextSkillPos(mSkillPos);
	std::vector<Skill*> skills = mGame->GetSkillSystem()->GetAcquiredSkills();
	float scale = 0.4f;

	for (auto skill : skills)
	{
		int i = 0;
		// 武器の発射までのクールダウンを割合で取得(マスクの描画に使う)
		float range = 1.0f -  player->GetWeaponIntervalRate(skill->GetName());
		if (skill->GetType() == Skill::Type::EWeapon)
		{
			++i;
			DrawTexture(shader, skill->GetIconTex(), nextSkillPos, scale);
			DrawTexture(shader, mIconMask, nextSkillPos, scale, range);
			Vector2 offset(Vector2(0.0f, skill->GetIconTex()->GetHeightF() * scale));
			Vector2 space(0.0f, 6.0f);
			nextSkillPos -= (offset + space);
		}
	} 
	Vector2 nextFramePos(mSkillPos);
	for (int i = 0; i < 5; ++i)
	{
		DrawTexture(shader, mIconFrame, nextFramePos);
		nextFramePos -= Vector2(0.0f, mIconFrame->GetHeightF() + 2.0f);
	}

	// タイマーの描画(2桁)
	float timer = phaseSystem->GetPhaseTimer();
	scale = 2.0f;		// タイマーの数字のスケール
	offset = Vector2(mNumbers[0]->GetWidthF() * scale, 0.0f);
	int tens = static_cast<int>(timer / 10);
	int ones = static_cast<int>(timer - tens * 10) % 10;
	if (!phaseSystem->OnTransition())
	{
		DrawTexture(shader, mNumbers[tens], mTimePos, scale);
		DrawTexture(shader, mNumbers[ones], mTimePos + offset, scale);
	}
	// Levelの描画
	DrawTexture(shader, mLevel, mLevelPos);
	int level = pParams.level;
	offset = Vector2(mLevel->GetWidthF(), 3.0f);
	tens = level / 10;
	ones = level % 10;
	DrawTexture(shader, mNumbers[tens], mLevelPos + offset);
	DrawTexture(shader, mNumbers[ones], mLevelPos + offset + Vector2(static_cast<float>(mNumbers[0]->GetWidth()), 0.0f));

	// Phaseの描画
	DrawTexture(shader, mPhase, mPhasePos);	// "Phase"
	offset = Vector2((mPhase->GetWidthF() + mNumbers[0]->GetWidthF()) / 2.0f, 3.0f);
	int phaseNum = phaseSystem->GetPhaseNum();
	DrawTexture(shader, mNumbers[phaseNum], mPhasePos + offset); // "1"とか"2"とか

	// 敵のHPbarの描画(HUDではないかもしれない)
	std::vector<EnemyActor*> enemies = mGame->GetEnemies();
	Renderer* renderer = mGame->GetRenderer();

	for (auto enemy : enemies)
	{
		// ワールド空間での位置を取得
		Vector3 enemyPos = enemy->GetHeadPosition();
		Vector3 offset(0.0f, 0.0f, 60.0f); // 敵の頭らへんにhpを表示したい
		Vector3 hpPosInWorld = enemyPos + offset;

		// カメラの背後にいる場合は描画しない
		if (!renderer->IsInFrontOfCamera(hpPosInWorld))
		{
			continue; // 次の敵へ
		}

		Vector2 screenPos = renderer->WorldToScreen(hpPosInWorld);

		// hpバーの背景を描画
		Vector2 hpPosInScreen(screenPos.x, screenPos.y);
		DrawTexture(shader, mHPbarBG, hpPosInScreen, 0.2f);

		// HPバーの色を判定して描画
		float hpDiscardRange = enemy->GetHpPercentage();
		if (hpDiscardRange >= 0.2f)
		{
			DrawTexture(shader, mHPbarGreen, hpPosInScreen, 0.2f, hpDiscardRange);
		}
		else if (hpDiscardRange >= 0.0f)
		{
			DrawTexture(shader, mHPbarRed, hpPosInScreen, 0.2f, hpDiscardRange);
		}
	}

	// Phase移行時「Excellent!!」の描画
	if (phaseSystem->OnTransition())
	{
		DrawTexture(shader, mExcellent, mSlider->GetPosition(), 1.0f);
		// Sliderが止まったら次のPhaseを始める
		if (mSlider->GetState() == UIAnimation::AnimState::EEnded)
		{
			delete mSlider;
			mSlider = nullptr;
			phaseSystem->StartPhase();
		}
	}
	// Phase移行時でない,且つ最初のPhaseでない時「NextPhase...を表示する」
	else if (phaseNum != 1)
	{
		if (timer <= 1.0f)
		{
			DrawTexture(shader, mNextPhase, Vector2(0.0f, 200.0f), 1.0f);
		}
		// テクスチャが右から徐々に消えていくように見える
		else if (timer <= 2.0f)
		{
			switch (phaseNum)
			{
			case 2:
				DrawTexture(shader, mNextPhase, Vector2(0.0f, 200.0f), 
				2.0f - timer);
				break;
			case 3:
				DrawTexture(shader, mNextPhase, Vector2(0.0f, 200.0f), 
				1.0f, 2.0f - timer);
				break;
			case 4:
				DrawTexture(shader, mNextPhase, Vector2(0.0f, 200.0f), 
				1.0f, 1.0f, 2.0f - timer);
				break;
			}
		}
	}
	Texture* mirror = mGame->GetRenderer()->GetMirrorTexture();
	DrawTexture(shader, mirror, Vector2(-350.0f, -250.0f), 1.0f, 1.0f, 1.0f, true);
}

void HUD::Reset()
{
	mHPdiscardRange = 1.0f;
	//mTimeFloat = 0.0f;
}

//void HUD::ResetTimer()
//{
//	mTimeFloat = 0.0f;
//}

void HUD::AddTargetComponent(TargetComponent* tc)
{
	mTargetComps.emplace_back(tc);
}

void HUD::RemoveTargetComponent(TargetComponent* tc)
{
	auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), tc);
	mTargetComps.erase(iter);
}

void HUD::ToNextPhase()
{
	float screenWidth = mGame->GetRenderer()->GetScreenWidth();
	float end = (screenWidth + mExcellent->GetWidthF()) * 1.0f / 2.0f;
	// 左から右へ流す
	mSlider = new Slider(end, 500.0f, Vector2(-end, 200.0f));
}

void HUD::UpdateRadar(float deltaTime)
{
	// １フレーム前の輝点位置をクリア
	mBlips.clear();

	// プレイヤーの位置をレーダー座標に変換(xが前方,yが上方)
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);
	// プレイヤーの前方ベクトルを変換
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);

	// レーダーの回転を求める
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// 2次元の回転行列を得る
	Matrix3 rotMat = Matrix3::CreateRotation(angle);

	// 輝点の位置を取得する
	for (auto tc : mTargetComps)
	{
		Vector3 targetPos = tc->GetOwner()->GetPosition();
		Vector2 actorPos2D(targetPos.y, targetPos.x);

		// プレイヤーからターゲットへのベクトルを計算
		Vector2 playerToTarget = actorPos2D - playerPos2D;

		// 範囲内にあるか
		if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
		{
			// playerToTargetを画面上のレーダーの中心からのオフセットへ変換
			Vector2 blipPos = playerToTarget;
			blipPos *= mRadarRadius / mRadarRange;

			// blipPosを回転する
			blipPos = Vector2::Transform(blipPos, rotMat);
			mBlips.emplace_back(blipPos);
		}
	}
}

void HUD::UpdateTimer(float deltaTime)
{
	//if (mGame->GetState() == Game::EGameplay)
	//{
	//	mTimeFloat += deltaTime;
	//	PhaseSystem* phaseSys = mGame->GetPhaseSystem();
	//	// Phaseを終えたら
	//	if (mTimeFloat >= phaseSys->GetMaxPhaseTime() + deltaTime)
	//	{
	//		phaseSys->ToNextPhase();
	//		if (mCurPhaseNum <= 4)
	//		{
	//			mCurPhaseNum++;
	//		}
	//	}
	//}
}
