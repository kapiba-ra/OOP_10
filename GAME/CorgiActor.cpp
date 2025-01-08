#include "CorgiActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "PhysWorld.h"

#include "Skeleton.h"
#include "Animation.h"

#include "SkeletalMeshComponent.h"
#include "ChaseMove.h"
#include "DamageComponent.h"
#include "BoxComponent.h"
#include "JumpComponent.h"

#include "EnemyActor.h"
#include "PlaneActor.h"


CorgiActor::CorgiActor(Game* game, EnemyActor* target)
	: WeaponActor(game)
	, mTarget(target)
{
	// mesh
	SkeletalMeshComponent* mc = new SkeletalMeshComponent(this);
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Corgi_backup.gpmesh");
	mc->SetMesh(mesh);
	mc->SetSkeleton(game->GetSkeleton("Assets/Corgi_backup.gpskel"));
	mc->PlayAnimation(game->GetAnimation("Assets/Corgi_walk.gpanim"));

	// move
	mMyMove = new ChaseMove(this, mTarget);

	// jump
	mJumpComp = new JumpComponent(this);

	// collision
	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(true);

	// �U���͍���
	mDamageComp->SetAttackDamage(5.0f);
}


	void CorgiActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollision();

	// �^�[�Q�b�g���L������Ȃ��Ȃ���or�^�[�Q�b�g��������->�^�[�Q�b�g���X�V����
	if ((!mTarget) || mTarget->GetUniState() == EnemyActor::UniState::EDying)
	{
		auto& enemies = GetGame()->GetEnemies();
		// ���̔z�񂩂�AUniState::Alive�̂��̂�I�Ԃ悤�ɂ�����
		for (auto enemy : enemies)
		{
			if (enemy->GetUniState() == EnemyActor::UniState::EAlive)
			{
				mTarget = enemy; // �^�[�Q�b�g��ݒ�
				mMyMove->SetTarget(mTarget);
				return; // �ŏ��Ɍ������G���^�[�Q�b�g�ɂ��ďI��
			}
		}
		// �����ɍ����G�����Ȃ���΁A�^�[�Q�b�g�� nullptr �ɐݒ�
		mTarget = nullptr;
		mMyMove->SetTarget(nullptr);
	}
}

void CorgiActor::FixCollision()
{
	ComputeWorldTransform();

	const AABB& corgiBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	Vector3 offset(0, 0, 50.0f);
	LineSegment line((pos + offset), pos + Vector3::UnitZ * -100.0f);
	bool inAir(true); // �󒆂ɂ��邩
	float t = 0.0f;				 // �����ݒ�p�ɕ֋X��K�v,�g��Ȃ�
	Vector3 norm(Vector3::Zero); // ����


	// �G�Ƃ̏Փ�
	GetGame()->GetPhysWorld()->TestSweepAndPrune([this, corgiBox](Actor* corgi, Actor* enemy)
	{
		if (corgi == this) // CorgiActor ���������Ă���ꍇ
		{
			// enemy��EnemyActor�̏ꍇ(Type�Ŕ��f)
			if (enemy->GetType() == Eenemy && enemy->GetState() == EActive)
			{
				// �G�Ƀq�b�g
				HitTarget(enemy);
			}
		}
	});

	// �ǁE���Ƃ̏Փ�
	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(corgiBox, planeBox))
		{
			float dx1 = planeBox.mMax.x - corgiBox.mMin.x;
			float dx2 = planeBox.mMin.x - corgiBox.mMax.x;
			float dy1 = planeBox.mMax.y - corgiBox.mMin.y;
			float dy2 = planeBox.mMin.y - corgiBox.mMax.y;
			float dz1 = planeBox.mMax.z - corgiBox.mMin.z;
			float dz2 = planeBox.mMin.z - corgiBox.mMax.z;

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
				if (dz == dz1)
				{
					if (mJumpComp->IsJumping())
					{
						mJumpComp->Land();
					}
				}
			}
			SetPosition(pos);
			// �킸���Ɉʒu�����炷�̂ňꉞ�ς��Ă���
			mBoxComp->OnUpdateWorldTransform();
			// ��������Ƃ̏Փ˂�,�����̑�������������������ɂ���ꍇ�ɃW�����v
			if ((pa->GetCategory() == PlaneActor::Category::EScaffold) &&
				(dz1 > 5.0f))
			{
				if (!mJumpComp->IsJumping())
				{
					mJumpComp->Liftoff(500.0f);
				}
			}
			if (!mJumpComp->IsJumping())
			{
				if (Intersect(line, planeBox, t, norm))
				{
					inAir = false;
				}
			}
		}
	}

	if (inAir && !mJumpComp->IsJumping())
	{
		// ���ؑ��x0�ŃW�����v������(���R����)
		mJumpComp->Liftoff(0.0f);
	}
}

void CorgiActor::SetTarget(EnemyActor* target)
{
	mTarget = target;
}

void CorgiActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
