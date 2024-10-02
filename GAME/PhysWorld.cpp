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
	// closestT�͖�����ŏ��������邱�Ƃ�,�ŏ��̌����ŕK���X�V�����
	float closestT = Math::Infinity;
	Vector3 norm;

	// �S�Ẵ{�b�N�X���e�X�g����
	for(auto box : mBoxes)
	{
		float t;
		// �����̓{�b�N�X�ƌ������邩
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// �O�̌��������߂���
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
	// �����Ȏ����ł���A�v�Z�ʂ� O(n^2)
	// ���[���h�ɂ���S�Ẵ{�b�N�X,������y�A�ɂ��Č����e�X�g���s��
	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// �������g�ƁA���Ƀe�X�g����i�̒l�Ƃ̓e�X�g���Ȃ�
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* a = mBoxes[i];
			BoxComponent* b = mBoxes[j];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				// �񋟂��ꂽ�֐����Ăяo���Č����𔻒肳����
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(class Actor*, class Actor*)> f)
{
	// min.x�ɂ���ă\�[�g����
	std::sort(mBoxes.begin(), mBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
			return a->GetWorldBox().mMin.x <
				b->GetWorldBox().mMin.x;
		});

	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// box[i]��max.x���擾
		BoxComponent* a = mBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* b = mBoxes[j];
			// ����box[j]��min.x��box[i]��max.x���E���z���Ă�����
			// box[i]�ƌ�������\��������{�b�N�X�͑��ɑ��݂��Ȃ�
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
