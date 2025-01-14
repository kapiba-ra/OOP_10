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
	, mPrevAnimation(nullptr)
	, mPrevAnimTime(0.0f)
	, mBlendTime(0.0f)
	, mBlendTimer(0.0f)
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

		// ブレンド中であれば経過時間を更新
		if (mPrevAnimation && mBlendTimer < mBlendTime)
		{
			mBlendTimer += deltaTime;
			if (mBlendTimer >= mBlendTime)
			{
				// ブレンドが終了したら前のアニメーションをリセット
				mPrevAnimation = nullptr;
				mBlendTimer = mBlendTime;
			}
		}

		// 行列パレットを再計算する
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate, float blendTime)
{
	mPrevAnimation = mAnimation;	 // 現在のアニメーションを保存(一回目に呼び出したときはnullptr)
	mPrevAnimTime = mAnimTime;       // 現在のアニメーション時間を保存

	mAnimation = anim;
	mAnimTime = 0.0f;
	mAnimPlayRate = playRate;
	mBlendTimer = 0.0f;        // 経過時間をリセット
	mBlendTime = blendTime;

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
	//const std::vector<Matrix4>& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();
	////std::vector<Matrix4> currentPoses;
	//mAnimation->GetGlobalPoseAtTime(mCurrentPoses, mSkeleton, mAnimTime);
	//
	//// 各ボーンのパレットを設定する
	//for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	//{
	//	mPalette.mEntry[i] = globalInvBindPoses[i] * mCurrentPoses[i];
	//}

	const auto& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();

	// 現在のアニメーションのポーズを計算
	std::vector<Matrix4> currentPoses;
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

	if (mPrevAnimation && mBlendTimer < mBlendTime)
	{
		// 前のアニメーションのポーズを計算
		std::vector<Matrix4> prevPoses;
		mPrevAnimation->GetGlobalPoseAtTime(prevPoses, mSkeleton, mPrevAnimTime);
		
		// ブレンド係数を計算
		float blendFactor = mBlendTimer / mBlendTime;

		// 全てのボーンを補間する
		for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
		{
			// 前のポーズと現在のポーズを取得
			const Matrix4& prevMatrix = prevPoses[i];
			const Matrix4& currentMatrix = currentPoses[i];

			// 補間した行列を格納する
			Matrix4 blendedMatrix;

			// 行列のすべての要素を補間(4*4行列)
			for (int row = 0; row < 4; ++row)
			{
				for (int col = 0; col < 4; ++col)
				{
					// 全部線形補間する
					blendedMatrix.mat[row][col] =
						prevMatrix.mat[row][col] * (1.0f - blendFactor) +
						currentMatrix.mat[row][col] * blendFactor;
					// 回転は線形だと正確ではないかもしれないが、動かしてみた感じだと
					// そんなに違和感はないので、これでいいのかもしれない。
				}
			}

			// 補間結果を現在のポーズに設定
			mCurrentPoses[i] = blendedMatrix;
		}
	}
	else
	{
		// ブレンドが終了している場合、現在のアニメーションのポーズを使用
		mCurrentPoses = currentPoses;
	}

	// ボーンごとの行列パレットを計算
	for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	{
		mPalette.mEntry[i] = globalInvBindPoses[i] * mCurrentPoses[i];
	}
}
