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

/// <summary>
/// 描画周りを担当するクラス
/// </summary>
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
	void SetMirrorView(const Matrix4& view) { mMirrorView = view; }
	class Texture* GetMirrorTexture() const { return mMirrorTexture; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

	void SetAmbientLight(const Vector3& ambient) { mAmbientLight = ambient; }
	DirectionalLight& GetDirectionalLight() { return mDirLight; }

	Vector3 Unproject(const Vector3& screenPoint) const;
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
	// 以下2つ追加
	// ワールド座標->スクリーン座標(-1から1に正規化してない座標系)
	Vector2 WorldToScreen(const Vector3& worldPos);
	// 与えられた座標がカメラの前側にあるかの判定
	bool IsInFrontOfCamera(const Vector3& worldPos);

private:
	bool LoadShaders();
	void CreateSpriteVerts();
	void SetLightUniforms(class Shader* shader);
	// バックミラーのフレームバッファ作成
	bool CreateMirrorTarget();

	void Draw3DScene(unsigned int framebuffer, const Matrix4& view,
		const Matrix4& proj, float viewPortScale = 1.0f);

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
	Matrix4 mMirrorView;	// ミラー用ビュー行列
	Matrix4 mProjection;	// 射影行列
	float mScreenWidth;
	float mScreenHeight;

	// ライティング用のデータ
	Vector3 mAmbientLight;		// 環境光
	DirectionalLight mDirLight; // 平行光源

	// バックミラー用のフレームバッファオブジェクト(FBO)のid
	unsigned int mMirrorBuffer;
	//バックミラーのテクスチャ
	class Texture* mMirrorTexture;

	SDL_Window* mWindow;
	SDL_GLContext mContext;
};

