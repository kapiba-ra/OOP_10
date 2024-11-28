#pragma once

class VertexArray
{
public:
	enum Layout
	{
		PosNormTex,		// ボーンなし(32byte)
		PosNormSkinTex	// ボーンあり(32+8=40byte)
	};
	VertexArray(const void* verts, unsigned int numVerts, Layout layout,
		const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	void SetActive();
	unsigned int GetNumVerts() const { return mNumVerts; }
	unsigned int GetNumIndices() const { return mNumIndices; }

private:
	unsigned int mNumVerts;		// 頂点数
	unsigned int mNumIndices;	// インデックスの数(ポリゴン数の3倍になるはず)
	// OpenGL ID
	unsigned int mVertexBuffer;
	unsigned int mIndexBuffer;
	unsigned int mVertexArray;
};

