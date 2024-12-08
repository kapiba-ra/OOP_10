#include "MeshComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner, bool isSkeletal)
	: Component(owner)
	, mMesh(nullptr)
	, mTextureIndex(0)
	, mVisible(true)
	, mIsSkeletal(isSkeletal)
{
	mOwner->GetGame()->GetRenderer()->AddMeshComp(this);
}

MeshComponent::~MeshComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveMeshComp(this);
}

void MeshComponent::Draw(Shader* shader)
{
	// この関数が呼び出された時点で既にシェーダーは設定されていることに注意
	if (mMesh)
	{
		// ワールド座標変換の設定
		shader->SetMatrixUniform(
			"uWorldTransform",
			mOwner->GetWorldTransform()
		);
		// 鏡面反射指数の設定
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// アクティブテクスチャの設定
		Texture* t = mMesh->GetTexture(mTextureIndex);
		// インデックスが有効ならばテクスチャを設定
		if (t) { t->SetActive(); }
		// メッシュの頂点配列をアクティブにする
		// 頂点の情報はシェーダーに渡すので必ず必要
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// 描画
		glDrawElements(
			GL_TRIANGLES,
			va->GetNumIndices(),
			GL_UNSIGNED_INT,
			nullptr
		);
	}
}
