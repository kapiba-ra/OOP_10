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
	std::vector<class Texture*> mTextures;	// Meshのテクスチャ
	class VertexArray* mVertexArray;		// Meshの頂点配列
	std::string mShaderName;
	float mRadius;	// オブジェクト空間での境界球の半径, Collisionに備えて
	float mSpecPower;	// 鏡面反射指数
	AABB mBox;
};

