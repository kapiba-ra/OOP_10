#include "Texture.h"
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>

Texture::Texture()
	: mTextureID(0)
	, mWidth(0)
	, mHeight(0)
{
}

Texture::~Texture()
{
}

bool Texture::Load(const std::string& fileName)
{
	int channels = 0; // 3...RGB, 4...RGBA
	unsigned char* image = SOIL_load_image(
		fileName.c_str(),
		&mWidth,		// 画像の幅
		&mHeight,		// 画像の高さ
		&channels,		// Number of channels is recorded
		SOIL_LOAD_AUTO	// Type of image file (png,jpeg...)
	);

	if (image == nullptr)
	{
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}

	int format = GL_RGB;
	if (channels == 4)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &mTextureID);	// Generate texture object
	glBindTexture(GL_TEXTURE_2D, mTextureID); // Activate Texture

	// 画像データをテクスチャオブジェクトにコピーする
	glTexImage2D(
		GL_TEXTURE_2D,		// Texture target
		0,					// Level of Detail
		format,				// Color format used by OpenGL
		mWidth,
		mHeight,
		0,					// Border color
		format,				// Color format of input data
		GL_UNSIGNED_BYTE,	// Bit depth of input data (8bits)
		image				// Pointer to the image
	);

	SOIL_free_image_data(image);	// unload

	// MipMap生成
	glGenerateMipmap(GL_TEXTURE_2D);

	// Enable 'bilinear filtering' (Not 'nearest-neighbor filtering')
	// 縮小フィルターでトライリニアフィルタリングを使う
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 異方性フィルタリングを有効にする
	// ここがないままだと、テクスチャを斜めに見た時にぼやけが発生する
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		GLfloat largest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
		// 有効にする
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
	}


	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTextureID);
}

void Texture::CreateFromSurface(SDL_Surface* surface)
{
	mWidth = surface->w;
	mHeight = surface->h;

	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::CreateForRendering(int width, int height, unsigned int format)
{
	mWidth = width;
	mHeight = height;

	// textureID の作成
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	// 画像の幅と高さを設定
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		mWidth,
		mHeight,
		0,
		GL_RGB,
		GL_FLOAT,
		nullptr		// 初期データはない(直前の引数と第2引数は無視される)
	);

	// レンダリング先のテクスチャには最近傍フィルタリングを使用
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}