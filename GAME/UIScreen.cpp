#include "UIScreen.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "Font.h"
#include "Shader.h"

UIScreen::UIScreen(Game* game)
	: mGame(game)
	, mTitle(nullptr)
	, mBackground(nullptr)
	, mTitlePos(0.0f, 300.0f)
	, mNextButtonPos(0.0f, 200.0f)
	, mState(EActive)
	, mBGPos(0.0f, 250.0f)
{
	// UIスタックにUIスクリーンを追加
	mGame->PushUI(this);
	mFont = mGame->GetFont("Assets/Carlito-Regular.ttf");
	mButtonOn = mGame->GetRenderer()->GetTexture("Assets/ButtonYellow.png");
	mButtonOff = mGame->GetRenderer()->GetTexture("Assets/ButtonBlue.png");

	for (int i = 0; i < 10; i++)
	{
		mNumbers[i] = mGame->GetRenderer()->GetTexture("Assets/Numbers/" + std::to_string(i) + ".png");
	}
}

UIScreen::~UIScreen()
{
	if (mTitle)
	{
		mTitle->Unload();
		delete mTitle;
	}
	for (auto b : mButtons)
	{
		delete b;
	}
	mButtons.clear();
}

void UIScreen::Update(float deltaTime)
{
}

void UIScreen::Draw(Shader* shader)
{
	// 背景を描画
	if (mBackground)
	{
		DrawTexture(shader, mBackground, mBGPos);
	}
	// タイトルを描画
	if (mTitle)
	{
		DrawTexture(shader, mTitle, mTitlePos);
	}
	// ボタンのテクスチャを描画してからボタンのテキストを描画する
	for (auto b : mButtons)
	{
		// ボタンの背景を描画
		Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
		DrawTexture(shader, tex, b->GetPosition());
		// ボタンのテキストを描画
		DrawTexture(shader, b->GetNameTex(), b->GetPosition());
	}
}

//void UIScreen::ProcessInput(const uint8_t* keys)
//{
//	// ボタンがあるならば
//	if (!mButtons.empty())
//	{
//		// マウスの位置
//		int x, y;
//		SDL_GetMouseState(&x, &y);	// (画面左上が原点の座標系)
//		// (0, 0)中心で1024×768の座標に変換する
//		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
//		mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
//		mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;
//
//		// ボタンを強調する
//		for (auto b : mButtons)
//		{
//			if (b->ContainPoint(mousePos))
//			{
//				b->SetHighlighted(true);
//			}
//			else
//			{
//				b->SetHighlighted(false);
//			}
//		}
//	}
//}

void UIScreen::ProcessInput(const InputState& state)
{
	// ボタンがあるならば
	if (!mButtons.empty())
	{
		// マウスの位置
		int x, y;
		SDL_GetMouseState(&x, &y);	// (画面左上が原点の座標系)
		// (0, 0)中心で1024×768の座標に変換する
		Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));
		mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
		mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f - mousePos.y;

		// ボタンを強調する
		for (auto b : mButtons)
		{
			if (b->ContainPoint(mousePos))
			{
				b->SetHighlighted(true);
			}
			else
			{
				b->SetHighlighted(false);
			}
		}
	}
}

void UIScreen::HandleKeyPress(int key)
{
	switch (key)
	{
	case SDL_BUTTON_LEFT:
		if (!mButtons.empty())
		{
			for (auto b : mButtons)
			{
				if (b->GetHighlighted())
				{
					b->OnClick();
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

void UIScreen::Reset()
{
}

void UIScreen::Close()
{
	mState = EClosing;
}

void UIScreen::SetTitle(const std::string& text, const Vector3& color, int pointSize)
{
	if (mTitle)
	{
		mTitle->Unload();
		delete mTitle;
		mTitle = nullptr;
	}
	mTitle = mFont->RenderText(text, color, pointSize);
}

void UIScreen::AddButton(const std::string& name, std::function<void()> onClick, Vector2 offset)
{
	Vector2 dims(static_cast<float>(mButtonOn->GetWidth()),
				 static_cast<float>(mButtonOn->GetHeight()));
	Button* b = new Button(name, mFont, onClick, mNextButtonPos, dims);
	mButtons.emplace_back(b);
	// 次のボタンの位置を更新
	// ボタンの高さ+余白の分だけ位置を下げる
	mNextButtonPos.y -= mButtonOff->GetHeight() + offset.y;
}

void UIScreen::DrawTexture(Shader* shader, Texture* texture, const Vector2& offset, float scale, float range)
{
	Matrix4 scaleMat = Matrix4::CreateScale(
		static_cast<float>(texture->GetWidth()) * scale,
		static_cast<float>(texture->GetHeight()) * scale,
		1.0f
	);
	Matrix4 transMat = Matrix4::CreateTranslation(Vector3(offset.x, offset.y, 0.0f));
	Matrix4 world = scaleMat * transMat;
	shader->SetMatrixUniform("uWorldTransform", world);
	shader->SetFloatUniform("uDiscardRange", range); // x軸方向の描画範囲を制限できる(hpの為に用意した)
	texture->SetActive();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void UIScreen::SetRelativeMouseMode(bool relative)
{
	if (relative)
	{
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_GetRelativeMouseState(nullptr, nullptr);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

Button::Button(const std::string& name, Font* font, std::function<void()> onClick, const Vector2& pos, const Vector2& dims)
	: mOnClick(onClick)
	, mNameTex(nullptr)
	, mFont(font)
	, mPosition(pos)
	, mDimensions(dims)
	, mHighlighted(false)
{
	SetName(name);
}

Button::~Button()
{
	if (mNameTex)
	{
		mNameTex->Unload();
		delete mNameTex;
	}
}

void Button::SetName(const std::string& name)
{
	mName = name;
	if (mNameTex)
	{
		mNameTex->Unload();
		delete mNameTex;
		mNameTex = nullptr;
	}
	mNameTex = mFont->RenderText(mName);
}

bool Button::ContainPoint(const Vector2& pt) const
{
	bool no = pt.x < (mPosition.x - mDimensions.x / 2.0f) ||
			  pt.x > (mPosition.x + mDimensions.x / 2.0f) ||
			  pt.y < (mPosition.y - mDimensions.y / 2.0f) ||
			  pt.y > (mPosition.y + mDimensions.y / 2.0f);
	return !no;
}

void Button::OnClick()
{
	// mOnClickハンドラがあれば(関数が定義されていれば)呼び出す
	if (mOnClick)
	{
		mOnClick();
	}
}
