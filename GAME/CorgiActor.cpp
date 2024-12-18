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

#include "EnemyActor.h"


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
}

void CorgiActor::SetTarget(EnemyActor* target)
{
	mTarget = target;
}

void CorgiActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}
