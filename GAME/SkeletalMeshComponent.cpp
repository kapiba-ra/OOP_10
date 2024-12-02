#include "SkeletalMeshComponent.h"
#include "Shader.h"
#include "Mesh.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Skeleton.h"
#include "Animation.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)
	: MeshComponent(owner, true)
	, mSkeleton(nullptr)
	, mAnimPlayRate(1.0f)
	, mAnimTime(0.0f)
	, mAnimation(nullptr)
{
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		// Set the world transform
		shader->SetMatrixUniform("uWorldTransform",
			mOwner->GetWorldTransform());
		// Set the matrix palette
		shader->SetMatrixUniforms("uMatrixPalette", &mPalette.mEntry[0],
			MAX_SKELETON_BONES);
		// Set specular power
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// Set the active texture
		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t)
		{
			t->SetActive();
		}
		// Set the mesh's vertex array as active
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// Draw
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	if (mAnimation && mSkeleton)
	{
		mAnimTime += deltaTime * mAnimPlayRate;
		// 長さを超えたら巻き戻す
		while (mAnimTime > mAnimation->GetDuration())
		{
			mAnimTime -= mAnimation->GetDuration();
		}

		// 行列パレットを再計算する
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate)
{
	mAnimation = anim;
	mAnimTime = 0.0f;
	mAnimPlayRate = playRate;

	if (!mAnimation)
	{
		return 0.0f;
	}
	ComputeMatrixPalette();
	return mAnimation->GetDuration();
}

Vector3 SkeletalMeshComponent::GetBonePosition(std::string boneName)
{
	Vector3 retVec(Vector3::Zero);
	// ボーンの名前からインデックスを取得
	int boneIndex = -1;
	const auto& bones = mSkeleton->GetBones();
	for (size_t i = 0; i < bones.size(); ++i)
	{
		if (bones[i].mName == boneName)
		{
			boneIndex = static_cast<int>(i);
			break;
		}
	}
	if (boneIndex >= 0 && boneIndex < static_cast<int>(mCurrentPoses.size()))
	{
		retVec = mCurrentPoses[boneIndex].GetTranslation();
	}
	return retVec;
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	//std::vector<Matrix4> currentPoses;
	mAnimation->GetGlobalPoseAtTime(mCurrentPoses, mSkeleton, mAnimTime);

	// 各ボーンのパレットを設定する
	for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	{
		mPalette.mEntry[i] = globalInvBindPoses[i] * mCurrentPoses[i];
	}
}
