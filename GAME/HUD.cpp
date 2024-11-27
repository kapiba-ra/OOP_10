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
	// prepare! �L���b�V�����Ă���,�ƕ\������
	PlayerActor* player = mGame->GetPlayer();
	PlayerActor::Parameters pParams = player->GetParams();
	Vector2 offset(Vector2::Zero);	// �g���܂킷

	// ���[�_�[�̕`��
	DrawTexture(shader, mRader, mRaderPos);
	// ���[�_�[��̋P�_�̕`��
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, mRaderPos + blip);
	}

	// HP�o�[�̕`��
	DrawTexture(shader, mHPbarBG, mHpbarPos);
	mHPdiscardRange = player->GetHpComp()->GetHpPercentage();
	if (mHPdiscardRange >= 0.0f)
	{
		DrawTexture(shader, mHPbar, mHpbarPos, 1.0f, mHPdiscardRange);
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

	// �^�C�}�[�̕`��(2��)
	scale = 2.0f;		// �^�C�}�[�̐����̃X�P�[��
	offset = Vector2(mNumbers[0]->GetWidthF() * scale, 0.0f);
	int tens = static_cast<int>(mTimeFloat / 10);
	int ones = static_cast<int>(mTimeFloat - tens * 10) % 10;
	DrawTexture(shader, mNumbers[tens], mTimePos, scale);
	DrawTexture(shader, mNumbers[ones], mTimePos + offset, scale);

	// Level�̕`��
	DrawTexture(shader, mLevel, mLevelPos);
	int level = pParams.level;
	offset = Vector2(mLevel->GetWidthF(), 3.0f);
	tens = level / 10;
	ones = level % 10;
	DrawTexture(shader, mNumbers[tens], mLevelPos + offset);
	DrawTexture(shader, mNumbers[ones], mLevelPos + offset + Vector2(static_cast<float>(mNumbers[0]->GetWidth()), 0.0f));

	// Phase�̕`��
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
		if (playerToTarget.LengthSq() <= (mRaderRange * mRaderRange))
		{
			// playerToTarget����ʏ�̃��[�_�[�̒��S����̃I�t�Z�b�g�֕ϊ�
			Vector2 blipPos = playerToTarget;
			blipPos *= mRaderRadius / mRaderRange;

			// blipPos����]����
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
		if (mTimeFloat >= 60.0f + deltaTime) // 60�ŕ\�������悤��
		{
			mGame->GetPhaseSystem()->ToNextPhase();
			if (mCurPhaseNum <= 4)
			{
				mCurPhaseNum++;
			}
		}
	}
}
