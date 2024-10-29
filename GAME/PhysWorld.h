#pragma once
#include <vector>
#include <functional>
#include "Math.h"
#include "Collision.h"

/* ���[���h�ɑ��݂���S�Ă�BoxComponent��c������B
* 
*/

class PhysWorld
{
public:
	PhysWorld(class Game* game);
	
	struct CollisionInfo
	{
		Vector3 mPoint;		// �Փ˂����ʒu
		Vector3 mNormal;	// �Փ˓_�̖@��
		class BoxComponent* mBox = nullptr;	// ���������R���|�[�l���g
		class Actor* mActor = nullptr;		// ���������R���|�[�l���g��owner�ł���Actor
	};

	// �������{�b�N�X�ƌ�������Ȃ�true��Ԃ�,CollisionInfo���Q�ƂŕԂ�
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// 
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// �X�C�[�v���v���[����@
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);


private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
};

