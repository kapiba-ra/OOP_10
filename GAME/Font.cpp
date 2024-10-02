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
	// �T�|�[�g����t�H���g�T�C�Y
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
	// �t�H���g�T�C�Y���Ƃ�TTF_OpenFont()���Ăяo���K�v������
	for (auto& size : fontSizes)
	{
		// TTF_OpenFont�͎w�肵���|�C���g�T�C�Y��TTF_Font�f�[�^�̃|�C���^��Ԃ�
		TTF_Font* font = TTF_OpenFont(fileName.c_str(), size);
		if (font == nullptr)
		{
			SDL_Log("�t�H���g %s �T�C�Y %d �̃��[�h�Ɏ��s", fileName.c_str(), size);
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
	// ��������󂯎��K�؂ȃT�C�Y�̃t�H���g���g���ăe�N�X�`�����쐬����
	// �t���[�����Ƃɂ��̃R�[�h���Ăяo���킯�ł͂Ȃ�

	Texture* texture = nullptr;
	// �F��SDL_Color��
	SDL_Color sdlColor;
	sdlColor.r = static_cast<Uint8>(color.x * 255);
	sdlColor.g = static_cast<Uint8>(color.y * 255);
	sdlColor.b = static_cast<Uint8>(color.z * 255);
	sdlColor.a = 255;
	// �w��T�C�Y�̃t�H���g�f�[�^��T��
	auto iter = mFontData.find(pointSize);
	// �t�H���g�f�[�^������������
	if (iter != mFontData.end())
	{
		TTF_Font* font = iter->second;
		const std::string& actualText = mGame->GetText(textKey);
		// SDL_Surface�ɕ`�悷��
		SDL_Surface* surf = TTF_RenderUTF8_Blended(font, actualText.c_str(), sdlColor);
		if (surf != nullptr)
		{
			// SDL_Surface����Texture�ւ̕ϊ�
			texture = new Texture();
			texture->CreateFromSurface(surf);
			SDL_FreeSurface(surf);
		}
	}
	else
	{
		SDL_Log("�|�C���g�T�C�Y %d �̓T�|�[�g����Ă��܂���", pointSize);
	}
	return texture;
}
