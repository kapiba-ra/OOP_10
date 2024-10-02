#pragma once
#include "Math.h"
#include <cstdint>
#include <string>
#include <functional>
#include <vector>

class Button
{
public:
	// コンストラクタは、ボタン名称・フォント・コールバック関数・
	// ボタンの位置と寸法を受け取る
	Button(const std::string& name, class Font* font,
		std::function<void()> onClick,
		const Vector2& pos, const Vector2& dims);
	~Button();

	// 表示テキストを設定し、テクスチャを生成する
	void SetName(const std::string& name);

	// 与えられた座標がボタンの範囲内ならtrueを返す
	bool ContainPoint(const Vector2& pt) const;
	// ボタンが押されたときに呼び出される
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
	Vector2 mPosition;   // 位置
	Vector2 mDimensions; // 寸法
	bool mHighlighted;
};

class UIScreen
{
public:
	UIScreen(class Game* game);
	virtual ~UIScreen();
	
	// UIScreenの派生クラスは以下をoverrideできる
	virtual void Update(float deltaTime);
	virtual void Draw(class Shader* shader);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key);

	// 状態管理
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
	// テクスチャを描画するヘルパー関数
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
	// 1024×768,中心(0,0)の座標で指定
	Vector2 mTitlePos;
	Vector2 mNextButtonPos;
	Vector2 mBGPos;

	UIState mState;
};

