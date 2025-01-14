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
		// �����𒴂����犪���߂�
		while (mAnimTime > mAnimation->GetDuration())
		{
			mAnimTime -= mAnimation->GetDuration();
		}

		// �u�����h���ł���Όo�ߎ��Ԃ��X�V
		if (mPrevAnimation && mBlendTimer < mBlendTime)
		{
			mBlendTimer += deltaTime;
			if (mBlendTimer >= mBlendTime)
			{
				// �u�����h���I��������O�̃A�j���[�V���������Z�b�g
				mPrevAnimation = nullptr;
				mBlendTimer = mBlendTime;
			}
		}

		// �s��p���b�g���Čv�Z����
		ComputeMatrixPalette();
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate, float blendTime)
{
	mPrevAnimation = mAnimation;	 // ���݂̃A�j���[�V������ۑ�(���ڂɌĂяo�����Ƃ���nullptr)
	mPrevAnimTime = mAnimTime;       // ���݂̃A�j���[�V�������Ԃ�ۑ�

	mAnimation = anim;
	mAnimTime = 0.0f;
	mAnimPlayRate = playRate;
	mBlendTimer = 0.0f;        // �o�ߎ��Ԃ����Z�b�g
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
	// �{�[���̖��O����C���f�b�N�X���擾
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
	//// �e�{�[���̃p���b�g��ݒ肷��
	//for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	//{
	//	mPalette.mEntry[i] = globalInvBindPoses[i] * mCurrentPoses[i];
	//}

	const auto& globalInvBindPoses = mSkeleton->GetGlobalInvBindPoses();

	// ���݂̃A�j���[�V�����̃|�[�Y���v�Z
	std::vector<Matrix4> currentPoses;
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

	if (mPrevAnimation && mBlendTimer < mBlendTime)
	{
		// �O�̃A�j���[�V�����̃|�[�Y���v�Z
		std::vector<Matrix4> prevPoses;
		mPrevAnimation->GetGlobalPoseAtTime(prevPoses, mSkeleton, mPrevAnimTime);
		
		// �u�����h�W�����v�Z
		float blendFactor = mBlendTimer / mBlendTime;

		// �S�Ẵ{�[�����Ԃ���
		for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
		{
			// �O�̃|�[�Y�ƌ��݂̃|�[�Y���擾
			const Matrix4& prevMatrix = prevPoses[i];
			const Matrix4& currentMatrix = currentPoses[i];

			// ��Ԃ����s����i�[����
			Matrix4 blendedMatrix;

			// �s��̂��ׂĂ̗v�f����(4*4�s��)
			for (int row = 0; row < 4; ++row)
			{
				for (int col = 0; col < 4; ++col)
				{
					// �S�����`��Ԃ���
					blendedMatrix.mat[row][col] =
						prevMatrix.mat[row][col] * (1.0f - blendFactor) +
						currentMatrix.mat[row][col] * blendFactor;
					// ��]�͐��`���Ɛ��m�ł͂Ȃ���������Ȃ����A�������Ă݂���������
					// ����ȂɈ�a���͂Ȃ��̂ŁA����ł����̂�������Ȃ��B
				}
			}

			// ��Ԍ��ʂ����݂̃|�[�Y�ɐݒ�
			mCurrentPoses[i] = blendedMatrix;
		}
	}
	else
	{
		// �u�����h���I�����Ă���ꍇ�A���݂̃A�j���[�V�����̃|�[�Y���g�p
		mCurrentPoses = currentPoses;
	}

	// �{�[�����Ƃ̍s��p���b�g���v�Z
	for (size_t i = 0; i < mSkeleton->GetNumBones(); ++i)
	{
		mPalette.mEntry[i] = globalInvBindPoses[i] * mCurrentPoses[i];
	}
}
