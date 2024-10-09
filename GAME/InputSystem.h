#pragma once
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_mouse.h>
#include "Math.h"

enum ButtonState
{
	ENone,		// 押されていない
	EPressed,	// 押された
	EReleased,	// 離された
	EHeld		// 押しっぱなし
};

class KeyboardState
{
public:
	friend class InputSystem;

	// キーの真偽値を取得
	bool GetKeyValue(SDL_Scancode keyCode) const;
	// ButtonStateの取得
	ButtonState GetKeyState(SDL_Scancode keyCode) const;

private:
	// 現在の状態
	const Uint8* mCurrState;
	// １フレーム前の状態
	Uint8* mPrevState[SDL_NUM_SCANCODES];
};

class MouseState
{
public:
	friend class InputSystem;

	const Vector2& GetPosition() const { return mMousePos; }
	const Vector2& GetScrollWheel() const { return mScrollWheel; }

	// マウスボタン用
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	Vector2 mMousePos;		// マウスの位置
	Uint32 mCurrButtons;	// 現在のボタン状態
	Uint32 mPrevButtons;	// ひとつ前のボタン状態
	Vector2 mScrollWheel;	// スクロールホイールの動き
	bool mIsRelative;		// 相対マウスモードかどうか
};

// 現在の入力状態を格納する
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

// friend class を使うことでInputSystemはprivateなメンバにアクセスできる
// InputSystemの中身をすっきりさせることができる