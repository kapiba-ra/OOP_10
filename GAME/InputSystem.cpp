#include "InputSystem.h"
#include <SDL/SDL.h>
#include <cstring>	// for memcpy,memset

bool InputSystem::Initialize()
{
	/* ----- キーボード ----- */
	// 現在の状態を示すポインタの保存
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// 前回の状態を示すメモリをクリア
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

	/* ----- マウス ----- */
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;

	return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
	// １フレーム前の状態を設定する(バッファにコピー)
	memcpy(
		mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES
	);
	// １フレーム前の状態を設定
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	mState.Mouse.mScrollWheel = Vector2::Zero;
}

void InputSystem::Update()
{
	// キーボードは特になし

	// マウス
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
	}

	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	// マウススクロール用
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y)
		);
		break;
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);
}

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons);
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}
