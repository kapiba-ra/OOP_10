#pragma once
#include <string>
#include <vector>
#include "Collision.h"

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool Load(const std::string& fileName, class Renderer* renderer);
	void Unload();

	class Texture* GetTexture(size_t index);
	class VertexArray* GetVertexArray() { return mVertexArray; }
	const std::string& GetShaderName() const { return mShaderName; }
	float GetRadius() const { return mRadius; }
	float GetSpecPower() const { return mSpecPower; }
	const AABB& GetBox() const { return mBox; }

private:
	std::vector<class Texture*> mTextures;	// Mesh�̃e�N�X�`��
	class VertexArray* mVertexArray;		// Mesh�̒��_�z��
	std::string mShaderName;
	float mRadius;	// �I�u�W�F�N�g��Ԃł̋��E���̔��a, Collision�ɔ�����
	float mSpecPower;	// ���ʔ��ˎw��
	AABB mBox;
};

