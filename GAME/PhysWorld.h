#pragma once
#include <vector>
#include <functional>
#include "Math.h"
#include "Collision.h"

/* ワールドに存在する全てのBoxComponentを把握する。
* 
*/

class PhysWorld
{
public:
	PhysWorld(class Game* game);
	
	struct CollisionInfo
	{
		Vector3 mPoint;		// 衝突した位置
		Vector3 mNormal;	// 衝突点の法線
		class BoxComponent* mBox = nullptr;	// 交差したコンポーネント
		class Actor* mActor = nullptr;		// 交差したコンポーネントのownerであるActor
	};

	// 線分がボックスと交差するならtrueを返す,CollisionInfoを参照で返す
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	// 
	void TestPairwise(std::function<void(class Actor*, class Actor*)> f);
	// スイープ＆プルーン手法
	void TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f);

	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);


private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
};

