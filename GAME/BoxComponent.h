#pragma once
#include "Component.h"
#include "Collision.h"

/* �A�N�^�[��AABB��ǉ�����R���|�[�l���g */

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
	AABB mObjectBox;	// �I�u�W�F�N�g��Ԃ�AABB
	AABB mWorldBox;		// ���[���h��Ԃ�AABB
	bool mShouldRotate;
};

