#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include <SDL/SDL.h>

DialogBox::DialogBox(Game* game, const std::string& text, std::function<void()> onOK, bool cancelButton)
	: UIScreen(game)
{
	// �_�C�A���O�{�b�N�X�p�Ɉʒu�𒲐߂���
	mBGPos = Vector2(0.0f, 0.0f);
	mTitlePos = Vector2(0.0f, 100.0f);
	mNextButtonPos = Vector2(0.0f, 0.0f);
	// �w�i�̃e�N�X�`����ݒ�
	mBackground = mGame->GetRenderer()->GetTexture("Assets/DialogBG.png");
	SetTitle(text, Vector3::Zero, 30);
	// �{�^���̐ݒ�
	AddButton("OKButton", [onOK]() {
		onOK();
	});
	if (cancelButton)
	{
		AddButton("CancelButton", [this]() {
			Close();
		});
	}
}

DialogBox::~DialogBox()
{
}
