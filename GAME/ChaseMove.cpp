#include "ChaseMove.h"

#include "Actor.h"
#include "Game.h"
#include "Collision.h"
#include "AStar.h"

#include "PlayerActor.h"
#include "PlaneActor.h"

#include "BoxComponent.h" // for planes box

ChaseMove::ChaseMove(Actor* owner)
	: MoveComponent(owner)
	, mChaseState(ETargeting)
	, mInterval(0.0f)
	, mNextPoint(Vector3::Zero)
{
	// ���̃R���|�[�l���g���A�^�b�`����Actor�������
	// Player�͐����ς݂łȂ��Ă͂Ȃ�Ȃ�
	mPlayer = mOwner->GetGame()->GetPlayer();
}

void ChaseMove::Update(float deltaTime)
{
	mInterval += deltaTime;

	// mChaseState��ύX���ׂ����ǂ������f����
	CheckPath(deltaTime);

	if (mChaseState == ETargeting)
	{
		Vector3 playerPos = mPlayer->GetPosition();
		TurnTo(playerPos);
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
			Vector3 playerPos = mPlayer->GetPosition();
			// ���ꂼ��player,owner�ɍł��߂��m�[�h���擾����
			WeightedGraphNode* myNode = FindClosestNode(*g, myPos);
			WeightedGraphNode* playerNode = FindClosestNode(*g, playerPos);
			bool found = AStarSearch(*g, myNode, playerNode, map);
			// start����goal�܂ł̏��ԂɊi�[����Ă���
			mPath = ReconstructPathCoords(myNode, playerNode, map);
			if (!mPath.empty())  // ��łȂ����Ƃ��m�F
			{
				mNextPoint = mPath.back();
			}
		}

		// ���B������A���̃|�C���g�֌����Č�����ς���...���J��Ԃ�
		Vector2 nextPoint2D(mNextPoint.x, mNextPoint.y);
		Vector2 myPos2D(myPos.x, myPos.y);
		//Vector3 diff = mNextPoint - myPos;
		Vector2 diff = nextPoint2D - myPos2D;
		if (diff.LengthSq() < 400.0f)
		{
			mPath.pop_back();
			if (!mPath.empty())  // ��łȂ����Ƃ��m�F
			{
				mNextPoint = mPath.back();
			}
		}
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

void ChaseMove::CheckPath(float deltaTime)
{
	ChaseState beforeCheck = mChaseState;
	// player�܂ł̊Ԃ̐��������
	Vector3 pos = mOwner->GetPosition();
	LineSegment line(pos, mPlayer->GetPosition());

	// ���ƕǂЂ�����߂�(���͏��̕������ʂ���,������2�K���Ăɂ��Ή��ł��邩��)
	auto& planes = mOwner->GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		float t = 0.0f;
		Vector3 norm(Vector3::Zero);
		const AABB& planeBox = pa->GetBox()->GetWorldBox();

		// ������player�Ƃ̊Ԃɏ����ǂ���������,Searching��Ԃ�
		if (Intersect(line, planeBox, t, norm))
		{
			mChaseState = ESearching;
			//if (mChaseState == ETargeting)
			if(beforeCheck == ETargeting)
			{
				OnEnter();
			}
			// �����ɂԂ����Ă�����I���
			return;
		}
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
