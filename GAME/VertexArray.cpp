#include "VertexArray.h"
#include "GL/glew.h"

VertexArray::VertexArray(const void* verts, unsigned int numVerts,
	Layout layout, const unsigned int* indices, unsigned int numIndices)
	: mNumVerts(numVerts)
	, mNumIndices(numIndices)
{
	// Create vertex array
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// 頂点のサイズ(ボーンなし : 32byte)
	unsigned vertexSize = 8 * sizeof(float);
	if (layout == PosNormSkinTex)
	{
		// ボーンあり : 40byte
		vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
	}

	// Create vertex buffer (on GPU)
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(
		GL_ARRAY_BUFFER,
		numVerts * vertexSize,
		verts,
		GL_STATIC_DRAW
	);

	// Create index buffer (on GPU)
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		numIndices * sizeof(unsigned int),
		indices,
		GL_STATIC_DRAW
	);

	// glVertexAttribPointerのラスト引数は,その属性の頂点データの開始位置を示している
	// 32byte(40byte)のどこからがその属性のデータなのか...
	if (layout == PosNormTex)	// ボーンなし
	{
		// Position : 3 float
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
		// NormalVec : 3 float 
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
			reinterpret_cast<void*>(sizeof(float) * 3));
		// Texture Coord : 2 float
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize,
			reinterpret_cast<void*>(sizeof(float) * 6));
	}
	else if (layout == PosNormSkinTex)	// ボーンあり
	{
		// Position : 3 float
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);
		// NormalVec : 3 float 
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
			reinterpret_cast<void*>(sizeof(float) * 3));		
		// スキニングのボーン : 4 int
		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(2, 4, GL_UNSIGNED_BYTE, vertexSize,
			reinterpret_cast<void*>(sizeof(float) * 6));
		// スキニングのウェイト : 4 float
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE,
			GL_TRUE,	// 数値を正規化する[0,255]->[0.0,1.0]
			vertexSize, reinterpret_cast<void*>(sizeof(float) * 6 + 4));
		// Texture Coord : 2 float
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, vertexSize,
			reinterpret_cast<void*>(sizeof(float) * 6 + 8));
	}
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
	glBindVertexArray(mVertexArray);
}
