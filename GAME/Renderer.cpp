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

	/* ----- OpenGLを使うウィンドウの属性を設定 ----- */
	// 使うプロファイルの設定(コアプロファイルを使う,ESはモバイル開発用)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// バージョンの設定(3.3)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBAで8ビットのカラーバッファを使う
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// Zバッファ(デプスバッファ)は24ビット
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// ダブルバッファを有効に
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// GPUを使う ハードウェアアクセラレーション
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// OpenGLの利用,作成されたWindowでOpenGLが使えるようになる
	mWindow = SDL_CreateWindow(
		"Game",	// Windowの名前
		100,		// Windowの表示位置(x座標)
		100,		// Windowの表示位置(y座標)
		static_cast<int>(mScreenWidth),		// Windowの幅
		static_cast<int>(mScreenHeight),	// Windowの高さ
		SDL_WINDOW_OPENGL	// SDL側からOpenGLで使えるWindowを作っている
	);
	if (!mWindow)
	{
		SDL_Log("Window作成失敗 : %s", SDL_GetError());
		return false;
	}
	// OpenGL Windowを使うためのコンテクストの作成
	mContext = SDL_GL_CreateContext(mWindow);
	// GLEW … OpenGLの拡張機能をロードするライブラリ。初期化する
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("GLEW初期化失敗");
		return false;
	}
	// 一部プラットフォームで無害なエラーを吐くのでクリアする
	glGetError();

	// LoadShader()はヘルパー関数
	if (!LoadShaders())
	{
		SDL_Log("シェーダーのロードに失敗.");
		return false;
	}

	// Spriteの描画先の四辺形(quadrilateral)の作成
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
	// テクスチャ削除
	for (auto i : mTextures)
	{
		i.second->Unload();
		delete i.second;
	}
	mTextures.clear();

	// メッシュ削除
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
	// Color buffer, Depth buffer をそれぞれクリア
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D 描画
	glEnable(GL_DEPTH_TEST);	// Depth Buffer を有効に
	glDisable(GL_BLEND);		// Alpha blending を無効に
	// Meshシェーダーをアクティブにしてビュー射影行列を更新
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

	// 2D 描画
	glDisable(GL_DEPTH_TEST);	// Depth Buffer を無効に
	glEnable(GL_BLEND);		// Alpha blending を有効に
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	// Spriteシェーダーと頂点配列オブジェクト(vao)をアクティブに
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	// 全てのスプライトを描画
	for (auto sprite : mSprites)
	{
		if (sprite->GetVisible())
		{
			sprite->Draw(mSpriteShader);
		}
	}

	// UIScreenを描画
	for (auto ui : mGame->GetUIStack())
	{
		ui->Draw(mSpriteShader);
	}

	// バッファ交換でシーンが表示される
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
	// screenPointを-1から+1までのデバイス座標(deviceCoord)に変換する
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= (mScreenWidth) * 0.5f;
	deviceCoord.y /= (mScreenHeight) * 0.5f;

	// 逆射影行列でベクトルを変換する
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// 始点を近接平面での画面の中心にする
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);

	// 終点を近接平面と遠方平面の間の画面の中心に定める
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);

	// 方向ベクトルを求める
	outDir = end - outStart;
	outDir.Normalize();
}

Vector2 Renderer::WorldToScreen(const Vector3& worldPos)
{
	// ビュー射影行列を渡す
	Vector3 screenPos = Vector3::TransformWithPerspDiv(worldPos, mView * mProjection);
	screenPos.x *= 0.5f * mScreenWidth;
	screenPos.y *= 0.5f * mScreenHeight;
	return Vector2(screenPos.x, screenPos.y);
}

bool Renderer::IsInFrontOfCamera(const Vector3& worldPos)
{
	// ビュー空間での位置を得る
	Vector3 viewSpacePos = Vector3::Transform(worldPos, mView);
	// ビュー空間でのz座標はカメラからの奥行きになる
	return viewSpacePos.z > 0.0f;  // カメラの前にいるならtrueを返す
}

bool Renderer::LoadShaders()
{
	/* ----- Spriteシェーダーの作成 ----- */
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}
	mSpriteShader->SetActive();
	/* ビュー射影行列をセット */
	// CreateSimpleViewProj()はワールド空間からクリップ空間への
	// 単純な変換,画面の解像度とワールド座標を1:1で対応させる
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	// このviewProjをSprite.vertファイルの"uniform mat4 uViewProj"に設定する
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	/* ----- Meshシェーダーの作成 ----- */
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	mMeshShader->SetActive();
	// ビュー行列
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,	// 目(カメラ)の位置
		Vector3::UnitX, // カメラを向ける方向
		Vector3::UnitZ  // 上方向
	);
	// 射影行列
	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.0f),	// 視野角(y)
		mScreenWidth,
		mScreenHeight,
		25.0f,		// near 
		10000.0f	// far
	);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);

	/* ----- SkinnedMeshシェーダーの作成 ----- */
	mSkinnedShader = new Shader();
	if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	mSkinnedShader->SetActive();
	// ビュー射影はMeshと同じ
	mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
	
	return true;
}

void Renderer::CreateSpriteVerts()
{
	// 1行1行が,一つの頂点が持つ情報
	// 位置3つ,法線3つ,テクスチャ座標2つ
	float vertices[] = {
		-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// 左上
		 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// 右上
		 0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// 右下
		-0.5f,-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f		// 左下
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	mSpriteVerts = new VertexArray(vertices, 4, VertexArray::PosNormTex, indices, 6);
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// uniform値の設定
	// ワールド空間におけるカメラの位置はビューの反転で求める
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());
	// 環境光
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);
	// 平行光源
	shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);
}
