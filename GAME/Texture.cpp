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
		&mWidth,		// �摜�̕�
		&mHeight,		// �摜�̍���
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

	// �摜�f�[�^���e�N�X�`���I�u�W�F�N�g�ɃR�s�[����
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

	// MipMap����
	glGenerateMipmap(GL_TEXTURE_2D);

	// Enable 'bilinear filtering' (Not 'nearest-neighbor filtering')
	// �k���t�B���^�[�Ńg���C���j�A�t�B���^�����O���g��
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// �ٕ����t�B���^�����O��L���ɂ���
	// �������Ȃ��܂܂��ƁA�e�N�X�`�����΂߂Ɍ������ɂڂ₯����������
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		GLfloat largest;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
		// �L���ɂ���
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

	// textureID �̍쐬
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	// �摜�̕��ƍ�����ݒ�
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		mWidth,
		mHeight,
		0,
		GL_RGB,
		GL_FLOAT,
		nullptr		// �����f�[�^�͂Ȃ�(���O�̈����Ƒ�2�����͖��������)
	);

	// �����_�����O��̃e�N�X�`���ɂ͍ŋߖT�t�B���^�����O���g�p
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}