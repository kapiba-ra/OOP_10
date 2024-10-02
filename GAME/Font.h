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

	bool Load(const std::string& fileName);
	void Unload();

	// ��������e�N�X�`���ɕ`�悷��
	class Texture* RenderText(
		const std::string& textKey,
		const Vector3& color = Color::White,
		int pointSize = 30
	);

private:
	class Game* mGame;
	// �|�C���g�T�C�Y�ƃt�H���g�f�[�^�̘A�z�z��
	std::unordered_map<int, TTF_Font*> mFontData;
};

