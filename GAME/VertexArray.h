#pragma once

class VertexArray
{
public:
	enum Layout
	{
		PosNormTex,		// �{�[���Ȃ�(32byte)
		PosNormSkinTex	// �{�[������(32+8=40byte)
	};
	VertexArray(const void* verts, unsigned int numVerts, Layout layout,
		const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	void SetActive();
	unsigned int GetNumVerts() const { return mNumVerts; }
	unsigned int GetNumIndices() const { return mNumIndices; }

private:
	unsigned int mNumVerts;		// ���_��
	unsigned int mNumIndices;	// �C���f�b�N�X�̐�(�|���S������3�{�ɂȂ�͂�)
	// OpenGL ID
	unsigned int mVertexBuffer;
	unsigned int mIndexBuffer;
	unsigned int mVertexArray;
};

