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
	// ���̊֐����Ăяo���ꂽ���_�Ŋ��ɃV�F�[�_�[�͐ݒ肳��Ă��邱�Ƃɒ���
	if (mMesh)
	{
		// ���[���h���W�ϊ��̐ݒ�
		shader->SetMatrixUniform(
			"uWorldTransform",
			mOwner->GetWorldTransform()
		);
		// ���ʔ��ˎw���̐ݒ�
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());
		// �A�N�e�B�u�e�N�X�`���̐ݒ�
		Texture* t = mMesh->GetTexture(mTextureIndex);
		// �C���f�b�N�X���L���Ȃ�΃e�N�X�`����ݒ�
		if (t) { t->SetActive(); }
		// ���b�V���̒��_�z����A�N�e�B�u�ɂ���
		// ���_�̏��̓V�F�[�_�[�ɓn���̂ŕK���K�v
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		// �`��
		glDrawElements(
			GL_TRIANGLES,
			va->GetNumIndices(),
			GL_UNSIGNED_INT,
			nullptr
		);
	}
}
