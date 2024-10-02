#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	: Component(owner)
	, mTexture(nullptr)
	, mDrawOrder(drawOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mVisible(true)
{
	mOwner->GetGame()->GetRenderer()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveSprite(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	if (mTexture)
	{
		// Scaling quadrilateral with Texture Width/Height
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight),
			1.0f
		);
		Matrix4 world = scaleMat * mOwner->GetWorldTransform();
		// Set WorldTransform
		shader->SetMatrixUniform("uWorldTransform", world);
		// Set texture
		mTexture->SetActive();
		// Draw quadrilateral
		glDrawElements(
			GL_TRIANGLES,		// Polygon type
			6,					// Number of indices in the index buffer
			GL_UNSIGNED_INT,	// Index type
			nullptr				// 通常nullptr
		);
	}
}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	// テクスチャの高さと幅を取得する
	mTexWidth = texture->GetWidth();
	mTexHeight = texture->GetHeight();
}
