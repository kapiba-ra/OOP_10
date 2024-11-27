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

	void SetActive();

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	/* static_cast<float>()...‚Á‚Ä‘‚­‚Æ’·‚¢‚©‚ç—pˆÓ... */
	// Get width as float
	float GetWidthF() const { return static_cast<float>(mWidth); }
	// Get height as float
	float GetmHeightF() const { return static_cast<float>(mHeight); }

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};

