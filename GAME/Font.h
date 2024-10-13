#pragma once
#include <string>
#include <unordered_map>
#include <SDL/SDL_ttf.h> // TrueType Font
#include "Math.h"

class Font
{
public:
	Font(class Game* game);
	~Font();

	// ファイルからフォントをLoad,Unloadする
	bool Load(const std::string& fileName);
	void Unload();

	// 文字列をテクスチャに描画する(テクスチャが'作成'される)
	class Texture* RenderText(
		const std::string& textKey,
		const Vector3& color = Color::White,
		int pointSize = 30
	);

private:
	class Game* mGame;
	// ポイントサイズとフォントデータの連想配列
	std::unordered_map<int, TTF_Font*> mFontData;
};

