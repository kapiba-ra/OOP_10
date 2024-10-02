#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>

class Button
{
public:
	// �R���X�g���N�^�́A�{�^�����́E�t�H���g�E�R�[���o�b�N�֐��E
	// �{�^���̈ʒu�Ɛ��@���󂯎��
	Button(const std::string& name, class Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	~Button();

	// �\���e�L�X�g��ݒ肵�A�e�N�X�`���𐶐�����
	void SetName(const std::string& name);

	// �^����ꂽ���W���{�^���͈͓̔��Ȃ�true��Ԃ�
	bool ContainPoint(const Vector2& pt) const;
	// �{�^���������ꂽ�Ƃ��ɌĂяo�����
	void OnClick();

	class Texture* GetNameTex() { return mNameTex; }
	const Vector2& GetPosition() const { return mPosition; }
	void SetHighlighted(bool sel) { mHighlighted = sel; }
	bool GetHighlighted() const { return mHighlighted; }

private:
	std::function<void()> mOnClick;
	std::string mName;
	class Texture* mNameTex;
	class Font* mFont;
	Vector2 mPosition;   // �ʒu
	Vector2 mDimensions; // ���@
	bool mHighlighted;
};

class UIScreen
{
public:
	UIScreen(class Game* game);
	virtual ~UIScreen();
	
	// UIScreen�̔h���N���X�͈ȉ���override�ł���
	virtual void Update(float deltaTime);
	virtual void Draw(class Shader* shader);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key);

	// ��ԊǗ�
	enum UIState
	{
		EActive,
		EClosing
	};
	void Close();
	UIState GetState() const { return mState; }
	
	void SetTitle(const std::string& text,
		const Vector3& color = Color::White,
		int pointSize = 40);

	void AddButton(const std::string& name, std::function<void()> onClick);

protected:
	// �e�N�X�`����`�悷��w���p�[�֐�
	void DrawTexture(class Shader* shader, class Texture* texture,
		const Vector2& offset = Vector2::Zero, float scale = 1.0f);
	void SetRelativeMouseMode(bool relative);
	class Game* mGame;

	class Font* mFont;
	class Texture* mTitle;
	class Texture* mButtonOn;
	class Texture* mButtonOff;
	class Texture* mBackground;
	
	std::vector<Button*> mButtons;
	// 1024�~768,���S(0,0)�̍��W�Ŏw��
	Vector2 mTitlePos;
	Vector2 mNextButtonPos;
	Vector2 mBGPos;

	UIState mState;
};

