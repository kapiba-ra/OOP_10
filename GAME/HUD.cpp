#include "HUD.h"
#include "Game.h"
#include "Renderer.h"
#include "PhysWorld.h"
#include "Texture.h"
#include "Actor.h"
#include "FPSActor.h"
#include "FollowActor.h"
#include "TargetComponent.h"

HUD::HUD(Game* game)
	: UIScreen(game)
	, mRaderRange(2000.0f)
	, mRaderRadius(92.0f)
	, mTargetEnemy(false)
{
	Renderer* r = mGame->GetRenderer();
	mRader = r->GetTexture("Assets/Radar.png");
	mCrosshair = r->GetTexture("Assets/Crosshair.png");
	mCrosshairEnemy = r->GetTexture("Assets/CrosshairRed.png");
	mBlipTex = r->GetTexture("Assets/Blip.png");
}

HUD::~HUD()
{
}

void HUD::Update(float deltaTime)
{
	UIScreen::Update(deltaTime);
	UpdateCrosshair(deltaTime);
	UpdateRadar(deltaTime);
}

void HUD::Draw(Shader* shader)
{
	// �\�����̕`��
	Texture* cross = mTargetEnemy ? mCrosshairEnemy : mCrosshair;
	DrawTexture(shader, cross, Vector2::Zero, 2.0f);

	// ���[�_�[�̕`��
	const Vector2 cRaderPos(-390.0f, 275.0f);
	DrawTexture(shader, mRader, cRaderPos, 1.0f);

	// �P�_�̕`��
	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, cRaderPos + blip, 1.0f);
	}
}

void HUD::AddTargetComponent(TargetComponent* tc)
{
	mTargetComps.emplace_back(tc);
}

void HUD::RemoveTargetComponent(TargetComponent* tc)
{
	auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), tc);
	mTargetComps.erase(iter);
}

void HUD::UpdateCrosshair(float deltaTime)
{
	// �G�ɏ\�������d�Ȃ��Ă��Ȃ���Ԃ̃J�[�\���ɖ߂�
	mTargetEnemy = false;
	// �����̍쐬
	const float cAimDist = 5000.0f;
	Vector3 start, dir;
	mGame->GetRenderer()->GetScreenDirection(start, dir);
	LineSegment l(start, start + dir * cAimDist);
	// �����L���X�g
	PhysWorld::CollisionInfo info;
	if (mGame->GetPhysWorld()->SegmentCast(l, info))
	{
		// �A�N�^�[��TargetComponent�������Ă���Ȃ�
		for (auto tc : mTargetComps)
		{
			if (tc->GetOwner() == info.mActor)
			{
				mTargetEnemy = true;
				break;
			}
		}
	}

}

void HUD::UpdateRadar(float deltaTime)
{
	// �P�t���[���O�̋P�_�ʒu���N���A
	mBlips.clear();

	// �v���C���[�̈ʒu�����[�_�[���W�ɕϊ�(x���O��,y�����)
	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);
	// �v���C���[�̑O���x�N�g����ϊ�
	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);

	// ���[�_�[�̉�]�����߂�
	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	// 2�����̉�]�s��𓾂�
	Matrix3 rotMat = Matrix3::CreateRotation(angle);

	// �P�_�̈ʒu���擾����
	for (auto tc : mTargetComps)
	{
		Vector3 targetPos = tc->GetOwner()->GetPosition();
		Vector2 actorPos2D(targetPos.y, targetPos.x);

		// �v���C���[����^�[�Q�b�g�ւ̃x�N�g�����v�Z
		Vector2 playerToTarget = actorPos2D - playerPos2D;

		// �͈͓��ɂ��邩
		if (playerToTarget.LengthSq() <= (mRaderRange * mRaderRange))
		{
			// playerToTarget����ʏ�̃��[�_�[�̒��S����̃I�t�Z�b�g�֕ϊ�
			Vector2 blipPos = playerToTarget;
			blipPos *= mRaderRadius / mRaderRange;

			// blipPos����]����
			blipPos = Vector2::Transform(blipPos, rotMat);
			mBlips.emplace_back(blipPos);
		}
	}
}
