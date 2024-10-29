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

	// Menu�̔w�i
	mBackground = mGame->GetRenderer()->GetTexture("Assets/LevelUpMenuBG.png");
	// Slide��Start����x���W�̐ݒ�
	float offset = mBackground->GetWidth() / 2.0f;
	float scLeftend = mGame->GetRenderer()->GetScreenWidth() / -2.0f;
	mSlideStartPosX = scLeftend - offset;
	mBGPos = Vector2(mSlideStartPosX, 0.0f);
	// �I������e�{�^���̔w�i
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
		// �X�s�[�h�A�b�v�̏���
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
	// �������Ȃ���\��������
	if (mSlideState == Slide::ESlideIn)
	{
		// ��0.33�b��UI���^�񒆂֓��B
		float moveAmount = mSlideSpeed * deltaTime;

		// �w�i�ƃ^�C�g���̈ړ�
		mBGPos.x += moveAmount;
		mTitlePos.x += moveAmount;
		// �{�^���ʒu�̈ړ�
		for (auto b : mButtons)
		{
			b->SetPosition(b->GetPosition() + Vector2(moveAmount, 0.0f));
		}

		if (mBGPos.x >= -moveAmount + mSlideEndPosX) // FadeEndPosX�͎���0�����ꉞ
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
	// ���j���[��ʂ̔w�i
	if (mBackground)
	{
		DrawTexture(shader, mBackground, mBGPos);
	}
	// ���j���[��ʂ̃^�C�g��
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	int i = 0;
	for (auto b : mButtons)
	{
		// �{�^���̔w�i��`��
		if (mButtonBGs[i])
		{
			DrawTexture(shader, mButtonBGs[i], b->GetPosition());
		}
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		// �{�^��(�t���[���I�Ȗ���)�ƃe�L�X�g��`��,�e�L�X�g���{�^���Ɉˑ����Ă銴��
		DrawTexture(shader, tex, b->GetPosition());
		DrawTexture(shader, b->GetNameTex(), b->GetPosition() + Vector2(50.0f, 0.0f));
		++i;
	}
}
