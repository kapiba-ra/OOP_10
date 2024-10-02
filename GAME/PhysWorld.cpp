#include "PhysWorld.h"
#include "BoxComponent.h"
#include <algorithm>
#include <SDL/SDL.h>

PhysWorld::PhysWorld(Game* game)
	: mGame(game)
{
}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
	bool collided = false;
	// closestTは無限大で初期化することで,最初の交差で必ず更新される
	float closestT = Math::Infinity;
	Vector3 norm;

	// 全てのボックスをテストする
	for(auto box : mBoxes)
	{
		float t;
		// 線分はボックスと交差するか
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// 前の交差よりも近いか
			if (t < closestT)
			{
				outColl.mPoint = l.PointOnSegment(t);
				outColl.mNormal = norm;
				outColl.mBox = box;
				outColl.mActor = box->GetOwner();
				collided = true;
			}
		}
	}
	return collided;
}

void PhysWorld::TestPairwise(std::function<void(class Actor*, class Actor*)> f)
{
	// 純粋な実装であり、計算量は O(n^2)
	// ワールドにある全てのボックス,あらゆるペアについて交差テストを行う
	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// 自分自身と、既にテストしたiの値とはテストしない
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* a = mBoxes[i];
			BoxComponent* b = mBoxes[j];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				// 提供された関数を呼び出して交差を判定させる
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f)
{
	// min.xによってソートする
	std::sort(mBoxes.begin(), mBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().mMin.x <
				b->GetWorldBox().mMin.x;
		});

	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// box[i]のmax.xを取得
		BoxComponent* a = mBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* b = mBoxes[j];
			// もしbox[j]のmin.xがbox[i]のmax.x境界を越えていたら
			// box[i]と交差する可能性があるボックスは他に存在しない
			if (b->GetWorldBox().mMin.x > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	mBoxes.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	auto iter = std::find(mBoxes.begin(), mBoxes.end(), box);
	if (iter != mBoxes.end())
	{
		std::iter_swap(iter, mBoxes.end() - 1);
		mBoxes.pop_back();
	}
}
