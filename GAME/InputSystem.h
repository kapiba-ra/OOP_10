#pragma once
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_mouse.h>
#include "Math.h"

enum ButtonState
{
	ENone,		// ������Ă��Ȃ�
	EPressed,	// �����ꂽ
	EReleased,	// �����ꂽ
	EHeld		// �������ςȂ�
};

class KeyboardState
{
public:
	friend class InputSystem;

	// �L�[�̐^�U�l���擾
	bool GetKeyValue(SDL_Scancode keyCode) const;
	// ButtonState�̎擾
	ButtonState GetKeyState(SDL_Scancode keyCode) const;

private:
	// ���݂̏��
	const Uint8* mCurrState;
	// �P�t���[���O�̏��
	Uint8* mPrevState[SDL_NUM_SCANCODES];
};

class MouseState
{
public:
	friend class InputSystem;

	const Vector2& GetPosition() const { return mMousePos; }
	const Vector2& GetScrollWheel() const { return mScrollWheel; }

	// �}�E�X�{�^���p
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	Vector2 mMousePos;		// �}�E�X�̈ʒu
	Uint32 mCurrButtons;	// ���݂̃{�^�����
	Uint32 mPrevButtons;	// �ЂƂO�̃{�^�����
	Vector2 mScrollWheel;	// �X�N���[���z�C�[���̓���
	bool mIsRelative;		// ���΃}�E�X���[�h���ǂ���
};

// ���݂̓��͏�Ԃ��i�[����
struct InputState
{
	KeyboardState Keyboard;
	MouseState Mouse;
};


class InputSystem
{
public:
	bool Initialize();
	void Shutdown();

	void PrepareForUpdate();
	void Update();
	void ProcessEvent(union SDL_Event& event);

	const InputState& GetState() const { return mState; }

	void SetRelativeMouseMode(bool value);

private:
	InputState mState;
};

// friend class ���g�����Ƃ�InputSystem��private�ȃ����o�ɃA�N�Z�X�ł���
// InputSystem�̒��g���������肳���邱�Ƃ��ł���