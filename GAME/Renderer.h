#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"

// ���s�����̍\����
struct DirectionalLight
{
	// ���̕���
	Vector3 mDirection;
	// �g�U���ːF
	Vector3 mDiffuseColor;
	// ���ʔ��ːF
	Vector3 mSpecColor;
};

class Renderer
{
public:
	Renderer(class Game* game);
	~Renderer();

	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();
	void UnloadData();

	void Draw();

	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	void AddMeshComp(class MeshComponent* mesh);
	void RemoveMeshComp(class MeshComponent* mesh);

	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	void SetViewMatrix(const Matrix4& view) { mView = view; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

	void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
	DirectionalLight& GetDirectionalLight() { return mDirLight; }

	Vector3 Unproject(const Vector3& screenPoint) const;
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;

private:
	bool LoadShaders();
	void CreateSpriteVerts();
	void SetLightUniforms(class Shader* shader);

	/*
	* mTextures      : map�ŊǗ�����
	* mMeshes        : load�����Mesh
	* mSprites		 : Sprite�̔z��
	* mMeshComps	 : �`�悳���Mesh(�{�[���Ȃ�)�̔z��
	* mSkeletalMeshs : �`�悳���SkeletalMesh�̔z��
	*/
	std::unordered_map<std::string, class Texture*> mTextures;
	std::unordered_map<std::string, class Mesh*> mMeshes;
	std::vector<class SpriteComponent*> mSprites;
	std::vector<class MeshComponent*> mMeshComps;
	std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;

	class Game* mGame;
	class Shader* mSpriteShader;
	class VertexArray* mSpriteVerts;
	class Shader* mMeshShader;		// ���ʂ̃��b�V���p�V�F�[�_�[
	class Shader* mSkinnedShader;	// �X�P���^�����b�V���p�V�F�[�_�[

	Matrix4 mView;			// �r���[�s��
	Matrix4 mProjection;	// �ˉe�s��
	float mScreenWidth;
	float mScreenHeight;

	// ���C�e�B���O�p�̃f�[�^
	Vector3 mAmbientLight;		// ����
	DirectionalLight mDirLight; // ���s����

	SDL_Window* mWindow;
	SDL_GLContext mContext;
};

