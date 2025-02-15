#include <GL/glew.h>
#include "Game.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "UIScreen.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "SpriteComponent.h"

Renderer::Renderer(Game* game)
	: mGame(game)
	, mSpriteShader(nullptr)
	, mMeshShader(nullptr)
	, mScreenHeight(768.0f)
	, mScreenWidth(1024.0f)
	, mSpriteVerts(nullptr)
	, mWindow(nullptr)
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	/* ----- OpenGL���g���E�B���h�E�̑�����ݒ� ----- */
	// �g���v���t�@�C���̐ݒ�(�R�A�v���t�@�C�����g��,ES�̓��o�C���J���p)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// �o�[�W�����̐ݒ�(3.3)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA��8�r�b�g�̃J���[�o�b�t�@���g��
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Z�o�b�t�@(�f�v�X�o�b�t�@)��24�r�b�g
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// �_�u���o�b�t�@��L����
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// GPU���g�� �n�[�h�E�F�A�A�N�Z�����[�V����
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// OpenGL�̗��p,�쐬���ꂽWindow��OpenGL���g����悤�ɂȂ�
	mWindow = SDL_CreateWindow(
		"Game",	// Window�̖��O
		100,		// Window�̕\���ʒu(x���W)
		100,		// Window�̕\���ʒu(y���W)
		static_cast<int>(mScreenWidth),		// Window�̕�
		static_cast<int>(mScreenHeight),	// Window�̍���
		SDL_WINDOW_OPENGL	// SDL������OpenGL�Ŏg����Window������Ă���
	);
	if (!mWindow)
	{
		SDL_Log("Window�쐬���s : %s", SDL_GetError());
		return false;
	}
	// OpenGL Window���g�����߂̃R���e�N�X�g�̍쐬
	mContext = SDL_GL_CreateContext(mWindow);
	// GLEW �c OpenGL�̊g���@�\�����[�h���郉�C�u�����B����������
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("GLEW���������s");
		return false;
	}
	// �ꕔ�v���b�g�t�H�[���Ŗ��Q�ȃG���[��f���̂ŃN���A����
	glGetError();

	// LoadShader()�̓w���p�[�֐�
	if (!LoadShaders())
	{
		SDL_Log("�V�F�[�_�[�̃��[�h�Ɏ��s.");
		return false;
	}

	// Sprite�̕`���̎l�ӌ`(quadrilateral)�̍쐬
	CreateSpriteVerts();

	return true;
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;
	mSpriteShader->Unload();
	delete mSpriteShader;
	mMeshShader->Unload();
	delete mMeshShader;
	mSkinnedShader->Unload();
	delete mSkinnedShader;
	SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
	// �e�N�X�`���폜
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// ���b�V���폜
	for (auto i : mMeshes)
	{
		i.second->Unload();
		delete i.second;
	}
	mMeshes.clear();
}

void Renderer::Draw()
{
	// Sky color
	glClearColor(0.8f, 0.9f, 0.9f, 1.0f);
	// Color buffer, Depth buffer �����ꂼ��N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D �`��
	glEnable(GL_DEPTH_TEST);	// Depth Buffer ��L����
	glDisable(GL_BLEND);		// Alpha blending �𖳌���
	// Mesh�V�F�[�_�[���A�N�e�B�u�ɂ��ăr���[�ˉe�s����X�V
	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	SetLightUniforms(mMeshShader);
	for (auto mc : mMeshComps)
	{
		if (mc->GetVisible())
		{
			mc->Draw(mMeshShader);
		}
	}
	mSkinnedShader->SetActive();
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
	SetLightUniforms(mSkinnedShader);
	for (auto sm : mSkeletalMeshes)
	{
		if (sm->GetVisible())
		{
			sm->Draw(mSkinnedShader);
		}
	}

	// 2D �`��
	glDisable(GL_DEPTH_TEST);	// Depth Buffer �𖳌���
	glEnable(GL_BLEND);		// Alpha blending ��L����
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Sprite�V�F�[�_�[�ƒ��_�z��I�u�W�F�N�g(vao)���A�N�e�B�u��
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	// �S�ẴX�v���C�g��`��
	for (auto sprite : mSprites)
	{
		if (sprite->GetVisible())
		{
			sprite->Draw(mSpriteShader);
		}
	}

	// UIScreen��`��
	for (auto ui : mGame->GetUIStack())
	{
		ui->Draw(mSpriteShader);
	}

	// �o�b�t�@�����ŃV�[�����\�������
	SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); iter++)
	{
		if (myDrawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}
	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end())
	{
		mSprites.erase(iter);
	}
}

void Renderer::AddMeshComp(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
		mSkeletalMeshes.emplace_back(sk);
	}
	else
	{
		mMeshComps.emplace_back(mesh);
	}
}

void Renderer::RemoveMeshComp(MeshComponent* mesh)
{
	if (mesh->GetIsSkeletal())
	{
		SkeletalMeshComponent* sk = static_cast<SkeletalMeshComponent*>(mesh);
		auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
		mSkeletalMeshes.erase(iter);
	}
	else
	{
		auto iter = std::find(mMeshComps.begin(), mMeshComps.end(), mesh);
		mMeshComps.erase(iter);
	}
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		tex = new Texture();
		if (tex->Load(fileName))
		{
			mTextures.emplace(fileName, tex);
		}
		else
		{
			delete tex;
			tex = nullptr;
		}
	}
	return tex;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
	Mesh* mesh = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		mesh = iter->second;
	}
	else
	{
		mesh = new Mesh();
		if (mesh->Load(fileName, this))
		{
			mMeshes.emplace(fileName, mesh);
		}
		else
		{
			delete mesh;
			mesh = nullptr;
		}
	}
	return mesh;
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// screenPoint��-1����+1�܂ł̃f�o�C�X���W(deviceCoord)�ɕϊ�����
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mScreenWidth) * 0.5f;
	deviceCoord.y /= (mScreenHeight) * 0.5f;

	// �t�ˉe�s��Ńx�N�g����ϊ�����
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// �n�_���ߐڕ��ʂł̉�ʂ̒��S�ɂ���
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);

	// �I�_���ߐڕ��ʂƉ������ʂ̊Ԃ̉�ʂ̒��S�ɒ�߂�
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);

	// �����x�N�g�������߂�
	outDir = end - outStart;
	outDir.Normalize();
}

Vector2 Renderer::WorldToScreen(const Vector3& worldPos)
{
	// �r���[�ˉe�s���n��
	Vector3 screenPos = Vector3::TransformWithPerspDiv(worldPos, mView * mProjection);
	screenPos.x *= 0.5f * mScreenWidth;
	screenPos.y *= 0.5f * mScreenHeight;
	return Vector2(screenPos.x, screenPos.y);
}

bool Renderer::IsInFrontOfCamera(const Vector3& worldPos)
{
	// �r���[��Ԃł̈ʒu�𓾂�
	Vector3 viewSpacePos = Vector3::Transform(worldPos, mView);
	// �r���[��Ԃł�z���W�̓J��������̉��s���ɂȂ�
	return viewSpacePos.z > 0.0f;  // �J�����̑O�ɂ���Ȃ�true��Ԃ�
}

bool Renderer::LoadShaders()
{
	/* ----- Sprite�V�F�[�_�[�̍쐬 ----- */
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}
	mSpriteShader->SetActive();
	/* �r���[�ˉe�s����Z�b�g */
	// CreateSimpleViewProj()�̓��[���h��Ԃ���N���b�v��Ԃւ�
	// �P���ȕϊ�,��ʂ̉𑜓x�ƃ��[���h���W��1:1�őΉ�������
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	// ����viewProj��Sprite.vert�t�@�C����"uniform mat4 uViewProj"�ɐݒ肷��
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	/* ----- Mesh�V�F�[�_�[�̍쐬 ----- */
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	mMeshShader->SetActive();
	// �r���[�s��
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,	// ��(�J����)�̈ʒu
		Vector3::UnitX, // �J���������������
		Vector3::UnitZ  // �����
	);
	// �ˉe�s��
	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.0f),	// ����p(y)
		mScreenWidth,
		mScreenHeight,
		25.0f,		// near 
		10000.0f	// far
	);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	/* ----- SkinnedMesh�V�F�[�_�[�̍쐬 ----- */
	mSkinnedShader = new Shader();
	if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	mSkinnedShader->SetActive();
	// �r���[�ˉe��Mesh�Ɠ���
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
	
	return true;
}

void Renderer::CreateSpriteVerts()
{
	// 1�s1�s��,��̒��_�������
	// �ʒu3��,�@��3��,�e�N�X�`�����W2��
	float vertices[] = {
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// ����
		 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// �E��
		 0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// �E��
		-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f		// ����
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// uniform�l�̐ݒ�
	// ���[���h��Ԃɂ�����J�����̈ʒu�̓r���[�̔��]�ŋ��߂�
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// ����
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// ���s����
	shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);
}
