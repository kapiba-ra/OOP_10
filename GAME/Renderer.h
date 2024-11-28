#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"

// 平行光源の構造体
struct DirectionalLight
{
	// 光の方向
	Vector3 mDirection;
	// 拡散反射色
	Vector3 mDiffuseColor;
	// 鏡面反射色
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
	* mTextures      : mapで管理する
	* mMeshes        : loadされるMesh
	* mSprites		 : Spriteの配列
	* mMeshComps	 : 描画されるMesh(ボーンなし)の配列
	* mSkeletalMeshs : 描画されるSkeletalMeshの配列
	*/
	std::unordered_map<std::string, class Texture*> mTextures;
	std::unordered_map<std::string, class Mesh*> mMeshes;
	std::vector<class SpriteComponent*> mSprites;
	std::vector<class MeshComponent*> mMeshComps;
	std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;

	class Game* mGame;
	class Shader* mSpriteShader;
	class VertexArray* mSpriteVerts;
	class Shader* mMeshShader;		// 普通のメッシュ用シェーダー
	class Shader* mSkinnedShader;	// スケルタルメッシュ用シェーダー

	Matrix4 mView;			// ビュー行列
	Matrix4 mProjection;	// 射影行列
	float mScreenWidth;
	float mScreenHeight;

	// ライティング用のデータ
	Vector3 mAmbientLight;		// 環境光
	DirectionalLight mDirLight; // 平行光源

	SDL_Window* mWindow;
	SDL_GLContext mContext;
};

