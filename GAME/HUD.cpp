#include "HUD.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "Texture.h"
#include "Actor.h"
//#include "FPSActor.h"
#include "PlayerActor.h"
#include "EnemyActor.h"
#include "TargetComponent.h"

HUD::HUD(Game* game)
	: UIScreen(game)
	, mRaderRange(2000.0f)
	, mRaderRadius(92.0f)
	, mHPdiscardRange(1.0f)
	, mTimeFloat(0.0f)
{
	Renderer* r = mGame->GetRenderer();
	mRader = r->GetTexture("Assets/Radar.png");
	mBlipTex = r->GetTexture("Assets/Blip.png");
	mHPbar = r->GetTexture("Assets/HPBar.png");
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
	// ���[�_�[�̕`��
	const Vector2 cRaderPos(-390.0f, 275.0f);
	DrawTexture(shader, mRader, cRaderPos, 1.0f);
	// ���[�_�[��̋P�_�̕`��
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, cRaderPos + blip, 1.0f);
	}

	// HP�o�[�̕`��
	if (mHPdiscardRange >= 0.0f)
	{
		DrawTexture(shader, mHPbar, Vector2(-350.0f, -350.0f), 1.0f, mHPdiscardRange);
	}

	// HP�̕`��
	Vector2 NumPos(-460.0f, -315.0f);
	Vector2 offset(20.0f, 0.0f);
	std::string hp = std::to_string(static_cast<int>(mGame->GetPlayer()->GetHP()));
	for (size_t i = 0;i < hp.length(); i++)
	{
		int digitIndex = hp[i] - '0';
		if (0 <= digitIndex && digitIndex <= 9) //�ꉞ�͈͊O�A�N�Z�X���Ȃ��悤��
		{
			DrawTexture(shader, mNumbers[digitIndex], NumPos + offset * static_cast<float>(i));
		}
	}

	NumPos = Vector2(-20.0f, 340.0f);
	offset = Vector2(40.0f, 0.0f);
	float scale = 2.0f;
	// �^�C�}�[�̕`��
	int tens = static_cast<int>(mTimeFloat / 10);
	int ones = static_cast<int>(mTimeFloat - tens * 10) % 10;
	DrawTexture(shader, mNumbers[tens], NumPos, scale);
	DrawTexture(shader, mNumbers[ones], NumPos + offset, scale);
}

void HUD::Reset()
{
	mHPdiscardRange = 1.0f;
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
		if (mTimeFloat >= 60.0f + deltaTime)
		{
			mGame->ChangeState(Game::EGameclear);
		}
	}
}
