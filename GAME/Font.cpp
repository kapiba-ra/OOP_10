#include "Font.h"
#include "Game.h"
#include "Texture.h"
#include <vector>

Font::Font(Game* game)
	: mGame(game)
{
}

Font::~Font()
{
}

bool Font::Load(const std::string& fileName)
{
	// サポートするフォントサイズ
	std::vector<int> fontSizes = {
		8, 9,
		10, 11, 12, 14, 16, 18,
		20, 22, 24, 26, 28,
		30, 32, 34, 36, 38,
		40, 42, 44, 46, 48,
		52, 56,
		60, 64, 68,
		72
	};
	// フォントサイズごとにTTF_OpenFont()を呼び出す必要がある
	for (auto& size : fontSizes)
	{
		// TTF_OpenFontは指定したポイントサイズのTTF_Fontデータのポインタを返す
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("フォント %s サイズ %d のロードに失敗", fileName.c_str(), size);
			return false;
		}
		mFontData.emplace(size, font);
	}
	return true;
}

void Font::Unload()
{
	for (auto& font : mFontData)
	{
		TTF_CloseFont(font.second);
	}
}

Texture* Font::RenderText(const std::string& textKey, const Vector3& color, int pointSize)
{
	// 文字列を受け取り適切なサイズのフォントを使ってテクスチャを作成する
	// フレームごとにこのコードを呼び出すわけではない

	Texture* texture = nullptr;
	// 色をSDL_Colorに
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;
	// 指定サイズのフォントデータを探す
	auto iter = mFontData.find(pointSize);
	// フォントデータが見つかったら
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		const std::string& actualText = mGame->GetText(textKey);
		// SDL_Surfaceに描画する
		SDL_Surface* surf = TTF_RenderUTF8_Blended(font, actualText.c_str(), sdlColor);
		if (surf != nullptr)
		{
			// SDL_SurfaceからTextureへの変換
			texture = new Texture();
			texture->CreateFromSurface(surf);
			SDL_FreeSurface(surf);
		}
	}
	else
	{
		SDL_Log("ポイントサイズ %d はサポートされていません", pointSize);
	}
	return texture;
}
