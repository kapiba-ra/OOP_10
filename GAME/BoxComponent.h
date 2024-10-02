#pragma once
#include "Component.h"
#include "Collision.h"

/* アクターにAABBを追加するコンポーネント */

class BoxComponent
	: public Component
{
public:
	BoxComponent(class Actor* owner, int updateOrder = 100);
	~BoxComponent();

	void OnUpdateWorldTransform() override;
	void SetObjectBox(const AABB& model) { mObjectBox = model; }
	const AABB& GetWorldBox() const { return mWorldBox; }
	void SetShouldRotate(bool value) { mShouldRotate = value; }

private:
	AABB mObjectBox;	// オブジェクト空間のAABB
	AABB mWorldBox;		// ワールド空間のAABB
	bool mShouldRotate;
};

