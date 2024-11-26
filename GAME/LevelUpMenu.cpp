#include "LevelUpMenu.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "SkillSystem.h"
#include <SDL/SDL.h>

#include "PlayerActor.h"
#include "HpComponent.h"

LevelUpMenu::LevelUpMenu(Game* game)
	: UIScreen(game)
	, mSlideState(Slide::ESlideIn)
	, mSlideEndPosX(0.0f)
	, mSlideSpeed(2000.0f)
{
	mSkillSystem = mGame->GetSkillSystem();

	// �{�^���̃e�N�X�`����ݒ�,Off�͓���,On�̓s���N�̘g
	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOn.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/LevelUpButtonOff.png");
	
	// Menu�ɕ\������e�L�X�g��ݒ�
	SetTitle("LevelUpTitle");
	
	SetRelativeMouseMode(false);

	// Menu�̔w�i
	mBackground = mGame->GetRenderer()->GetTexture("Assets/LevelUpMenuBG.png");
	// Slide��Start����x���W�̐ݒ�
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
	// ���j���[��ʂ̃^�C�g��(LevelUP!)
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	int i = 0;
	for (auto b : mButtons)
	{
		// �{�^���̃A�C�R����`��
		if (mButtonBGs[i] && mIcons[i])
		{
			DrawTexture(shader, mButtonBGs[i], b->GetPosition());
			DrawTexture(shader, mIcons[i], b->GetPosition() - Vector2(165.0f, 0.0f));
		}
		++i;
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		// �{�^��(�s���N�̘g�݂�����,�J�[�\������ĂȂ��Ɖ����Ȃ�)�ƃe�L�X�g��`��
		// �e�L�X�g�ƃ{�^���̓Z�b�g�ɂȂ��Ă���
		DrawTexture(shader, tex, b->GetPosition());
		DrawTexture(shader, b->GetNameTex(), b->GetPosition() + Vector2(50.0f, 0.0f), 0.5f);
	}
}

void LevelUpMenu::AddButtonRandom()
{
	Vector2 buttonOffset = Vector2(0.0f, 10.0f);

	PlayerActor* player = mGame->GetPlayer();

	std::vector<Skill*> skills = mSkillSystem->GetRandomSkills();
	for (auto skill : skills)
	{
		// BG�̕����͕ς�邩��
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/SkillBG.png"));
		mIcons.emplace_back(skill->GetIconTex());
		AddButton(skill->GetName(), [this, skill, player]() {
			mGame->ChangeState(Game::EGameplay);
			skill->LevelUp(player);
			Close();
		}, buttonOffset);
	}
	// �����o���ĂȂ����ۂ�
	if (skills.size() == 0)
	{
		mButtonBGs.emplace_back(mGame->GetRenderer()->GetTexture("Assets/RecoverBG.png"));
		AddButton("Recover", [this, player]() {
			mGame->ChangeState(Game::EGameplay);
			player->GetHpComp()->Recover(20.0f);
			Close();
		}, buttonOffset);
	}
}
