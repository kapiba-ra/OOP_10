#include "HUD.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "Texture.h"
#include "Font.h"
#include "PhaseSystem.h"
#include "SkillSystem.h"

#include "Actor.h"
#include "PlayerActor.h"
#include "EnemyActor.h"

#include "TargetComponent.h"
#include "HpComponent.h"

HUD::HUD(Game* game)
	: UIScreen(game)
	, mRaderRange(2000.0f)
	, mRaderRadius(92.0f)
	, mHPdiscardRange(1.0f)
	, mTimeFloat(0.0f)
	, mCurPhaseNum(1)
	, mRaderPos(-390.0f, 275.0f)
	, mHpbarPos(-350.0f, -350.0f)
	, mHpNumPos(-460.0f, -315.0f)
	, mTimePos(-20.0f, 340.0f)
	, mLevelPos(400.0f, 350.0f)
	, mPhasePos(300.0f, 350.0f)
	, mSkillPos(-480.0f, 0.0f)
{
	Renderer* r = mGame->GetRenderer();
	mRader = r->GetTexture("Assets/Radar.png");
	mBlipTex = r->GetTexture("Assets/Blip.png");
	mHPbarBG = r->GetTexture("Assets/HPBarBG.png");
	mHPbar = r->GetTexture("Assets/HPBar.png");
	mIconFrame = r->GetTexture("Assets/Icon/Frame.png");
	mLevel = mFont->RenderText("LevelText");
	mPhase = mFont->RenderText("PhaseText");
}

HUD::~HUD()
{
}

void HUD::Update(float deltaTime)
{
	UIScreen::Update(deltaTime);
	UpdateRadar(deltaTime);
	UpdateTimer(deltaTime);
}

void HUD::Draw(Shader* shader)
{
	// prepare! キャッシュしておく,と表現する
	PlayerActor* player = mGame->GetPlayer();
	PlayerActor::Parameters pParams = player->GetParams();
	Vector2 offset(Vector2::Zero);	// 使いまわす

	// レーダーの描画
	DrawTexture(shader, mRader, mRaderPos);
	// レーダー上の輝点の描画
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, mRaderPos + blip);
	}

	// HPバーの描画
	DrawTexture(shader, mHPbarBG, mHpbarPos);
	mHPdiscardRange = player->GetHpComp()->GetHpPercentage();
	if (mHPdiscardRange >= 0.0f)
	{
		DrawTexture(shader, mHPbar, mHpbarPos, 1.0f, mHPdiscardRange);
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
		if (skill->GetType() == Skill::Type::EWeapon)
		{
			DrawTexture(shader, skill->GetIconTex(), nextSkillPos, scale);
			Vector2 offset(Vector2(0.0f, skill->GetIconTex()->GetmHeightF() * scale));
			Vector2 space(0.0f, 6.0f);
			nextSkillPos -= (offset + space);
		}
	} 
	Vector2 nextFramePos(mSkillPos);
	for (int i = 0; i < 5; ++i)
	{
		DrawTexture(shader, mIconFrame, nextFramePos);
		nextFramePos -= Vector2(0.0f, mIconFrame->GetmHeightF() + 2.0f);
	}

	// タイマーの描画(2桁)
	scale = 2.0f;		// タイマーの数字のスケール
	offset = Vector2(mNumbers[0]->GetWidthF() * scale, 0.0f);
	int tens = static_cast<int>(mTimeFloat / 10);
	int ones = static_cast<int>(mTimeFloat - tens * 10) % 10;
	DrawTexture(shader, mNumbers[tens], mTimePos, scale);
	DrawTexture(shader, mNumbers[ones], mTimePos + offset, scale);

	// Levelの描画
	DrawTexture(shader, mLevel, mLevelPos);
	int level = pParams.level;
	offset = Vector2(mLevel->GetWidthF(), 3.0f);
	tens = level / 10;
	ones = level % 10;
	DrawTexture(shader, mNumbers[tens], mLevelPos + offset);
	DrawTexture(shader, mNumbers[ones], mLevelPos + offset + Vector2(static_cast<float>(mNumbers[0]->GetWidth()), 0.0f));

	// Phaseの描画
	DrawTexture(shader, mPhase, mPhasePos);
	offset = Vector2((mPhase->GetWidthF() + mNumbers[0]->GetWidthF()) / 2.0f, 3.0f);
	DrawTexture(shader, mNumbers[mCurPhaseNum], mPhasePos + offset);
}

void HUD::Reset()
{
	mHPdiscardRange = 1.0f;
	mTimeFloat = 0.0f;
}

void HUD::ResetTimer()
{
	mTimeFloat = 0.0f;
}

void HUD::AddTargetComponent(TargetComponent* tc)
{
	mTargetComps.emplace_back(tc);
}

void HUD::RemoveTargetComponent(TargetComponent* tc)
{
	auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), tc);
	mTargetComps.erase(iter);
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
		if (playerToTarget.LengthSq() <= (mRaderRange * mRaderRange))
		{
			// playerToTargetを画面上のレーダーの中心からのオフセットへ変換
			Vector2 blipPos = playerToTarget;
			blipPos *= mRaderRadius / mRaderRange;

			// blipPosを回転する
			blipPos = Vector2::Transform(blipPos, rotMat);
			mBlips.emplace_back(blipPos);
		}
	}
}

void HUD::UpdateTimer(float deltaTime)
{
	if (mGame->GetState() == Game::EGameplay)
	{
		mTimeFloat += deltaTime;
		if (mTimeFloat >= 60.0f + deltaTime) // 60で表示されるように
		{
			mGame->GetPhaseSystem()->ToNextPhase();
			if (mCurPhaseNum <= 4)
			{
				mCurPhaseNum++;
			}
		}
	}
}
