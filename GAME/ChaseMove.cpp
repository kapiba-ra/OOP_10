#include "ChaseMove.h"

#include "Actor.h"
#include "Game.h"
#include "Collision.h"
#include "PhysWorld.h"
#include "AStar.h"

#include "PlayerActor.h"
#include "PlaneActor.h"

#include "BoxComponent.h" // for planes box

ChaseMove::ChaseMove(Actor* owner, Actor* target)
	: MoveComponent(owner)
	, mChaseState(ETargeting)
	, mInterval(0.0f)
	, mNextPoint(Vector3::Zero)
	, mTarget(target)
{
}

void ChaseMove::Update(float deltaTime)
{
	mInterval += deltaTime;

	// mChaseState��ύX���ׂ����ǂ������f����
	CheckObstacle(deltaTime);

	Vector3 targetPos(Vector3::Zero);
	if(mTarget) targetPos = mTarget->GetPosition();
	if (mChaseState == ETargeting)
	{
		TurnTo(targetPos);
	}
	else if (mChaseState == ESearching)
	{
		Vector3 myPos = mOwner->GetPosition();
		// 10�b�҂��čČv�Z����
		if (mInterval >= 10.0f)
		{
			mInterval = 0.0f;
			mPath.clear();

			Vector3 myPos = mOwner->GetPosition();

			AStarMap map;
			WeightedGraph* g = mOwner->GetGame()->GetGraph();
			// ���ꂼ��target,owner�ɍł��߂��m�[�h���擾����
			WeightedGraphNode* myNode = FindClosestNode(*g, myPos);
			WeightedGraphNode* targetNode = FindClosestNode(*g, targetPos);
			bool found = AStarSearch(*g, myNode, targetNode, map);
			// start����goal�܂ł̏��ԂɊi�[����Ă���
			mPath = ReconstructPathCoords(myNode, targetNode, map);
			if (!mPath.empty())  // ��łȂ����Ƃ��m�F
			{
				mNextPoint = mPath.back();
			}
		}

		// ���B������A���̃|�C���g�֌����Č�����ς���...���J��Ԃ�
		Vector2 nextPoint2D(mNextPoint.x, mNextPoint.y);
		Vector2 myPos2D(myPos.x, myPos.y);
		Vector3 diff3D = mNextPoint - myPos;
		Vector2 diff2D = nextPoint2D - myPos2D;
		if (diff2D.LengthSq() < 10000.0f)
		{
			if (!mPath.empty())
			{
				mPath.pop_back();
			}
			if (!mPath.empty())  // ��łȂ����Ƃ��m�F
			{
				mNextPoint = mPath.back();
			}
		}
		// ���̃|�C���g������
		TurnTo(mNextPoint);
	}

	MoveComponent::Update(deltaTime);
}

void ChaseMove::TurnTo(const Vector3& pos)
{
	// �e�N���X�̃����o�ϐ��ł���mAngularSpeed�̐�����s��
	Vector3 dir = Vector3(pos - mOwner->GetPosition());
	dir.z = 0.0f;	// z�����͍l���Ȃ�
	dir.Normalize();
	Vector3 forward = mOwner->GetForward();
	float dot = Vector3::Dot(forward, dir);
	Vector3 cross = Vector3::Cross(forward, dir); // ���Ȃ�E��]���ׂ�

	// ��̓������������Ă�Ȃ�(Targeting�ł��鎖���O��)
	if (dot > 0.95f)
	{
		SetAngularSpeed(0.0f);
	}
	else if (cross.z > 0.0f)
	{
		SetAngularSpeed(Math::Pi);
	}
	else if (cross.z <= 0.0f)
	{
		SetAngularSpeed(-Math::Pi);
	}
}

void ChaseMove::SetTarget(Actor* target)
{
	mTarget = target;
	mPath.clear();
	mInterval = 10.0f;
}

void ChaseMove::CheckObstacle(float deltaTime)
{
	/* ���FPlaneActor�������̊Ԃɂ��邩�̔���̍ۂ�,����PlaneActor�𔻒�
	* ���邱�Ƃ�����B�������2��,��͐����̎n�_������肿����Ə�ɂ��邱��,
	* �������,PlaneActor����Actor�Ȃ�..�Ƃ���������������邱��
	* �O�҂̕����킾���܂肪���Ȃ����Ȃ̂�,�O�҂��̗p���Ă���
	*/

	ChaseState stateBeforeCheck = mChaseState;
	// player�܂ł̊Ԃ̐��������
	// pos���������Ə��ƐڐG���Ă锻��ɂȂ邱�Ƃ�������Ȃ̂�,z������0.1�v���X���Ă���
	Vector3 pos = mOwner->GetPosition() + Vector3(0.0f, 0.0f, 0.1f);
	Vector3 targetPos(Vector3::Zero);
	if(mTarget) targetPos = mTarget->GetPosition();
	// targetPos�͑�̑����ŁA���̕t�߂̕����s���������̂�z������+100.0f���Ă���
	LineSegment line(pos, targetPos);

	// ���ƕǂЂ�����߂�(���͏��̕������ʂ���,������2�K���Ăɂ��Ή��ł��邩��)
	auto& planes = mOwner->GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		float t = 0.0f;
		Vector3 norm(Vector3::Zero);
		const AABB& planeBox = pa->GetBox()->GetWorldBox();

		// ������player�Ƃ̊Ԃɕǂ����ꂪ��������,Searching��Ԃ�
		// Floor���ǂ����̔���͍s���Ă��Ȃ��B
		if ((Intersect(line, planeBox, t, norm))) //&&
			//(pa->GetCategory() != (PlaneActor::Category::EFloor)))
		{
			mChaseState = ESearching;
			if(stateBeforeCheck == ETargeting)
			{
				OnEnter();
			}
			// �����ɂԂ����Ă����珈�����I���
			return;
		}
		//PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
		//PhysWorld::CollisionInfo info;
		//phys->TestSweepAndPrune([](Actor* a, Actor* b) {
		//	// �^�O�ŏ�������
		//	if (a->GetType() == Actor::Type::Eplane)
		//	{
		//		static_cast<PlaneActor*>(a);
		//	}
		//});
	}


	// �������Ȃ�������ETargeting��
	mChaseState = ETargeting;
}

void ChaseMove::OnEnter()
{
	switch (mChaseState)
	{
	case ETargeting:
	{
		break;
	}
	case ESearching:
	{
		mInterval = 10.0f;
		break;
	}
	}
}
