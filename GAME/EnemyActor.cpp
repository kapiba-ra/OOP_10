#include "EnemyActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "InputSystem.h"

//#include "MoveComponent.h"
#include "ChaseMove.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "TargetComponent.h"

#include "FollowActor.h"
#include "PlaneActor.h"
// ui

EnemyActor::EnemyActor(Game* game)
	: Actor(game, Type::Eenemy)
	, mMoveComp(nullptr)
	, mMeshComp(nullptr)
	, mBoxComp(nullptr)
	, mHP(1.0f)
	, mTimer(0.0f)
{
	mMeshComp = new MeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Human.gpmesh");
	mMeshComp->SetMesh(mesh);
	// TODO: �����_��spawn�ɂ������BGame�N���X�Őݒ�
	SetPosition(Vector3(200.0f, 200.0f, 0.0f));
	// ���b�V���ɂ��
	SetScale(40.0f);

	mMyMove = new ChaseMove(this);
	// ��ɐi�ݑ�����
	mMyMove->SetForwardSpeed(100.0f);

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);

	new TargetComponent(this);
}

void EnemyActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	if (mHP <= 0.0f)
	{
		SetState(EDead);
	}

	FixCollisions();
	
}

void EnemyActor::FixCollisions()
{
	// �ۂ���FollowActor�ɂ����Ă���������
	ComputeWorldTransform();

	const AABB& enemyBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	auto player = GetGame()->GetPlayer();
	const AABB& playerBox = player->GetBox()->GetWorldBox();
	if (Intersect(enemyBox, playerBox))
	{
		// �G�̈ʒu���v���C���[���班������
		
		// �v���C���[��HP���ւ炷
		player->TakeDamage(1.0f);
	}

	// �ǁE���Ƃ̏Փ�
	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(enemyBox, planeBox))
		{
			float dx1 = planeBox.mMax.x - enemyBox.mMin.x;
			float dx2 = planeBox.mMin.x - enemyBox.mMax.x;
			float dy1 = planeBox.mMax.y - enemyBox.mMin.y;
			float dy2 = planeBox.mMin.y - enemyBox.mMax.y;
			float dz1 = planeBox.mMax.z - enemyBox.mMin.z;
			float dz2 = planeBox.mMin.z - enemyBox.mMax.z;

			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
			}
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}
