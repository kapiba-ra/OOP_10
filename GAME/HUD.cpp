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
	// �����B�L���b�V�����Ă����ƕ\������
	PlayerActor* player = mGame->GetPlayer();
	PlayerActor::Parameters pParams = player->GetParams();
	Vector2 offset(Vector2::Zero);	// �g���܂킷
	PhaseSystem* phaseSystem = mGame->GetPhaseSystem();

	// ���[�_�[�̕`��
	DrawTexture(shader, mRadar, mRadarPos);
	// ���[�_�[��̋P�_�̕`��
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, mRadarPos + blip);
	}
	DrawTexture(shader, mPlayerInRadar, mRadarPos);

	// HP�o�[�̕`��
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
	// HP(number)�̕`��
	offset = Vector2(mNumbers[0]->GetWidthF(), 0.0f);
	std::string hp = std::to_string(static_cast<int>(player->GetHpComp()->GetCurHp()));
	for (size_t i = 0;i < hp.length(); i++)
	{
		int digitIndex = hp[i] - '0';
		if (0 <= digitIndex && digitIndex <= 9) //�ꉞ�͈͊O�A�N�Z�X���Ȃ��悤��
		{
			DrawTexture(shader, mNumbers[digitIndex], mHpNumPos + offset * static_cast<float>(i));
		}
	}

	// �X�L���̃A�C�R���̕`��
	Vector2 nextSkillPos(mSkillPos);
	std::vector<Skill*> skills = mGame->GetSkillSystem()->GetAcquiredSkills();
	float scale = 0.4f;

	for (auto skill : skills)
	{
		int i = 0;
		// ����̔��˂܂ł̃N�[���_�E���������Ŏ擾(�}�X�N�̕`��Ɏg��)
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

	// �^�C�}�[�̕`��(2��)
	float timer = phaseSystem->GetPhaseTimer();
	scale = 2.0f;		// �^�C�}�[�̐����̃X�P�[��
	offset = Vector2(mNumbers[0]->GetWidthF() * scale, 0.0f);
	int tens = static_cast<int>(timer / 10);
	int ones = static_cast<int>(timer - tens * 10) % 10;
	if (!phaseSystem->OnTransition())
	{
		DrawTexture(shader, mNumbers[tens], mTimePos, scale);
		DrawTexture(shader, mNumbers[ones], mTimePos + offset, scale);
	}
	// Level�̕`��
	DrawTexture(shader, mLevel, mLevelPos);
	int level = pParams.level;
	offset = Vector2(mLevel->GetWidthF(), 3.0f);
	tens = level / 10;
	ones = level % 10;
	DrawTexture(shader, mNumbers[tens], mLevelPos + offset);
	DrawTexture(shader, mNumbers[ones], mLevelPos + offset + Vector2(static_cast<float>(mNumbers[0]->GetWidth()), 0.0f));

	// Phase�̕`��
	DrawTexture(shader, mPhase, mPhasePos);	// "Phase"
	offset = Vector2((mPhase->GetWidthF() + mNumbers[0]->GetWidthF()) / 2.0f, 3.0f);
	int phaseNum = phaseSystem->GetPhaseNum();
	DrawTexture(shader, mNumbers[phaseNum], mPhasePos + offset); // "1"�Ƃ�"2"�Ƃ�

	// �G��HPbar�̕`��(HUD�ł͂Ȃ���������Ȃ�)
	std::vector<EnemyActor*> enemies = mGame->GetEnemies();
	Renderer* renderer = mGame->GetRenderer();

	for (auto enemy : enemies)
	{
		// ���[���h��Ԃł̈ʒu���擾
		Vector3 enemyPos = enemy->GetHeadPosition();
		Vector3 offset(0.0f, 0.0f, 60.0f); // �G�̓���ւ��hp��\��������
		Vector3 hpPosInWorld = enemyPos + offset;

		// �J�����̔w��ɂ���ꍇ�͕`�悵�Ȃ�
		if (!renderer->IsInFrontOfCamera(hpPosInWorld))
		{
			continue; // ���̓G��
		}

		Vector2 screenPos = renderer->WorldToScreen(hpPosInWorld);

		// hp�o�[�̔w�i��`��
		Vector2 hpPosInScreen(screenPos.x, screenPos.y);
		DrawTexture(shader, mHPbarBG, hpPosInScreen, 0.2f);

		// HP�o�[�̐F�𔻒肵�ĕ`��
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

	// Phase�ڍs���uExcellent!!�v�̕`��
	if (phaseSystem->OnTransition())
	{
		DrawTexture(shader, mExcellent, mSlider->GetPosition(), 1.0f);
		// Slider���~�܂����玟��Phase���n�߂�
		if (mSlider->GetState() == UIAnimation::AnimState::EEnded)
		{
			delete mSlider;
			mSlider = nullptr;
			phaseSystem->StartPhase();
		}
	}
	// Phase�ڍs���łȂ�,���ŏ���Phase�łȂ����uNextPhase...��\������v
	else if (phaseNum != 1)
	{
		if (timer <= 1.0f)
		{
			DrawTexture(shader, mNextPhase, Vector2(0.0f, 200.0f), 1.0f);
		}
		// �e�N�X�`�����E���珙�X�ɏ����Ă����悤�Ɍ�����
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
	// ������E�֗���
	mSlider = new Slider(end, 500.0f, Vector2(-end, 200.0f));
}

void HUD::UpdateRadar(float deltaTime)
{
	// �P�t���[���O�̋P�_�ʒu���N���A
	mBlips.clear();

	// �v���C���[�̈ʒu�����[�_�[���W�ɕϊ�(x���O��,y�����)
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);
	// �v���C���[�̑O���x�N�g����ϊ�
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);

	// ���[�_�[�̉�]�����߂�
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// 2�����̉�]�s��𓾂�
	Matrix3 rotMat = Matrix3::CreateRotation(angle);

	// �P�_�̈ʒu���擾����
	for (auto tc : mTargetComps)
	{
		Vector3 targetPos = tc->GetOwner()->GetPosition();
		Vector2 actorPos2D(targetPos.y, targetPos.x);

		// �v���C���[����^�[�Q�b�g�ւ̃x�N�g�����v�Z
		Vector2 playerToTarget = actorPos2D - playerPos2D;

		// �͈͓��ɂ��邩
		if (playerToTarget.LengthSq() <= (mRadarRange * mRadarRange))
		{
			// playerToTarget����ʏ�̃��[�_�[�̒��S����̃I�t�Z�b�g�֕ϊ�
			Vector2 blipPos = playerToTarget;
			blipPos *= mRadarRadius / mRadarRange;

			// blipPos����]����
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
	//	// Phase���I������
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
