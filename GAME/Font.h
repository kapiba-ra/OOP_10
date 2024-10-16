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

// 一応作った
namespace FontSize
{
	static const int font_8 = 8;
	static const int font_9 = 9;
	static const int font_10 = 10;
	static const int font_11 = 11;
	static const int font_12 = 12;
	static const int font_14 = 14;
	static const int font_16 = 16;
	static const int font_18 = 18;
	static const int font_20 = 20;
	static const int font_22 = 22;
	static const int font_24 = 24;
	static const int font_26 = 26;
	static const int font_28 = 28;
	static const int font_30 = 30;
	static const int font_32 = 32;
	static const int font_34 = 34;
	static const int font_36 = 36;
	static const int font_38 = 38;
	static const int font_40 = 40;
	static const int font_42 = 42;
	static const int font_44 = 44;
	static const int font_46 = 46;
	static const int font_48 = 48;
	static const int font_52 = 52;
	static const int font_56 = 56;
	static const int font_60 = 60;
	static const int font_64 = 64;
	static const int font_68 = 68;
	static const int font_72 = 72;
}