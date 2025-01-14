#include "EnemyActor.h"
#include <random>
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "InputSystem.h"

#include "ChaseMove.h"
//#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "BoxComponent.h"
#include "TargetComponent.h"
#include "HpComponent.h"
#include "DamageComponent.h"
#include "JumpComponent.h"

#include "PlayerActor.h"
#include "PlaneActor.h"
#include "ExpActor.h"

EnemyActor::EnemyActor(Game* game)
	: Actor(game, Type::Eenemy)
	, mUniState(UniState::EAlive)
	, mInvincibleDuration(0.5f)	// ���G����,�����ݒ��0.5�b
	, mInvincibilityTimer(0.0f)
	, mMaxJumpSpeed(500.0f)
{
	mMeshComp = new SkeletalMeshComponent(this);
	// �l�^
	//Mesh* mesh = game->GetRenderer()->GetMesh("Assets/GP_Human.gpmesh");
	//mMeshComp->SetMesh(mesh);
	//mMeshComp->SetSkeleton(game->GetSkeleton("Assets/GP_Human.gpskel"));
	//mMeshComp->PlayAnimation(game->GetAnimation("Assets/GP_HumanWalk.gpanim"));
	// �X���C��
	//Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Slime.gpmesh");
	//mMeshComp->SetMesh(mesh);
	//mMeshComp->SetSkeleton(game->GetSkeleton("Assets/Slime.gpskel"));
	//mMeshComp->PlayAnimation(game->GetAnimation("Assets/SlimeIdle.gpanim"));

	// �o���ʒu�̓����_���ȃm�[�h����
	WeightedGraph* g = game->GetGraph();
	size_t size = g->mNodes.size();
	std::random_device rd;    // �V�[�h�𐶐�
	std::mt19937 gen(rd());   // �����Z���k�E�c�C�X�^����������
	std::uniform_int_distribution<> dist(0, size - 1);  // �O���t�m�[�h���͈̔͂ŗ����𐶐�

	int randomIndex = dist(gen);
	WeightedGraphNode* node = g->mNodes[randomIndex];
	// NoAccess�ȊO��������܂Ń��[�v
	while (node->type == NodeType::ENoAccess)
	{
		randomIndex = dist(gen);
		node = g->mNodes[randomIndex];
	}
	SetScale(100.0f);
	SetPosition(node->NodePos);

	//if (node->type != NodeType::ENoAccess)
	//{
	//	SetPosition(node->NodePos);
	//}
	
	
	mMyMove = new ChaseMove(this, game->GetPlayer());
	// ��ɐi�ݑ�����
	mMyMove->SetForwardSpeed(150.0f);

	mJumpComp = new JumpComponent(this);

	// Damage(�U����),Hp���Ƀf�t�H���g��,1
	mDamageComp = new DamageComponent(this);
	mHpComp = new HpComponent(this, 1.0f);

	// ���[�_�[�ւ̕\���Ɏg��
	new TargetComponent(this);

	game->AddEnemy(this);
}

EnemyActor::~EnemyActor()
{
	GetGame()->RemoveEnemy(this);
}

void EnemyActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mInvincibilityTimer += deltaTime;	// ���G���ԗp�̃^�C�}�[�͂����ŉ��Z��������

	if (mUniState == UniState::EDying)
	{
		float scale = GetScale() - 1.0f;
		SetScale(scale);
		if (scale <= 0.0f)
		{
			SetState(EDead);
			// �A�C�e���𗎂Ƃ�
			Actor* a = new ExpActor(GetGame());
			// �A�C�e���̈ʒu�͑����̂�����Ə�
			Vector3 offset(0.0f, 0.0f, 50.0f);
			a->SetPosition(GetPosition() + offset);
		}
	}
	// �|���ꂽ�u�Ԃ̏���
	else if (mHpComp->IsKilled() && mUniState == UniState::EAlive)
	{
		mUniState = UniState::EDying;
		mMyMove->SetForwardSpeed(0.0f);
		PlayDyingAnimation();
	}

	FixCollisions();
}

void EnemyActor::Reset()
{
	// �V���v���ɏ���(����������)�B
	SetState(EDead);
}

void EnemyActor::FixCollisions()
{
	ComputeWorldTransform();

	const AABB& enemyBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();
	Vector3 offset(0, 0, 50.0f);
	LineSegment line((pos + offset), pos + Vector3::UnitZ * -100.0f);
	bool inAir(true); // �󒆂ɂ��邩
	float t = 0.0f;				 // �����ݒ�p�ɕ֋X��K�v,�g��Ȃ�
	Vector3 norm(Vector3::Zero); // ����

	auto player = GetGame()->GetPlayer();
	const AABB& playerBox = player->GetBox()->GetWorldBox();
	// Enemy������Ă���Ƃ��͓����蔻��𔭐������Ȃ�(�V��ł�����ăA�h�o�C�X)
	if (Intersect(enemyBox, playerBox) &&
		//player->GetState() == EActive &&
		mUniState != UniState::EDying)
	{
		// �G�̈ʒu���v���C���[���班������
		
		// �v���C���[��HP���ւ炷
		player->GetHpComp()->TakeDamage(1.0f);
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
				(dz1>5.0f))
			{
				if (!mJumpComp->IsJumping())
				{
					mJumpComp->Liftoff(mMaxJumpSpeed);
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
	// �f�o�b�O���₷���悤��
	bool isJumping = mJumpComp->IsJumping();
	if (inAir && !isJumping)
	{
		// ���ؑ��x0�ŃW�����v������(���R����)
		mJumpComp->Liftoff(0.0f);
	}
}

void EnemyActor::TakeDamage(float amount)
{
	if (mInvincibilityTimer >= mInvincibleDuration)
	{
		mHpComp->TakeDamage(amount);
		mInvincibilityTimer = 0.0f;
	}
}

void EnemyActor::SetSpeed(float speed)
{
	mMyMove->SetForwardSpeed(speed);
}

Vector3 EnemyActor::GetHeadPosition()
{
	Vector3 headPos = mMeshComp->GetBonePosition("head") + Vector3(0.0f, 0.0f, 1.0f);
	Matrix4 worldTransform = GetWorldTransform();
	return Vector3::Transform(headPos, worldTransform);
}

Slime::Slime(Game* game)
	: EnemyActor(game)
{
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/Slime.gpmesh");
	mMeshComp->SetMesh(mesh);
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/Slime.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/SlimeIdle.gpanim"));

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);
}

void Slime::PlayDyingAnimation()
{
	mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/SlimeDying.gpanim"));
}

Zombie::Zombie(Game* game)
	: EnemyActor(game)
{
	Mesh* mesh = game->GetRenderer()->GetMesh("Assets/GP_Human.gpmesh");
	mMeshComp->SetMesh(mesh);
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/GP_Human.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/GP_HumanWalk.gpanim"));

	mBoxComp = new BoxComponent(this);
	mBoxComp->SetObjectBox(mesh->GetBox());
	mBoxComp->SetShouldRotate(false);
}

void Zombie::PlayDyingAnimation()
{
	mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/GP_HumanDying.gpanim"));
}
