#pragma once
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void Unload();

	void CreateFromSurface(struct SDL_Surface* surface);
	// �e�N�X�`���ւ̃����_�����O����������֐�
	void CreateForRendering(int width, int height, unsigned int format);

	void SetActive();

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	/* static_cast<float>()...���ď����ƒ�������p��... */
	// Get width as float
	float GetWidthF() const { return static_cast<float>(mWidth); }
	// Get height as float
	float GetHeightF() const { return static_cast<float>(mHeight); }
	unsigned int GetTextureID() const { return mTextureID; }

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};

