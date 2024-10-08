#include "FollowActor.h"
#include "MeshComponent.h"
#include "MoveComponent.h"
#include "BoxComponent.h"
#include "FollowCamera.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
// for collision caluculation
#include "PlaneActor.h"

FollowActor::FollowActor(Game* game)
	: Actor(game)
	, mBoxComp(nullptr)
{
	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));
	SetPosition(Vector3(0.0f, 0.0f, -50.0f));
	SetScale(100.0f);

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
	
	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f,0.5f,0.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FollowActor::ActorInput(const uint8_t* keys)
{
	float forwardSpeed = 0.0f;	// 前進速度
	float angularSpeed = 0.0f;	// 回転速度
	// 前後と回転の移動
	if (keys[SDL_SCANCODE_W])
	{
		forwardSpeed += 400.0f;
	}
	if (keys[SDL_SCANCODE_S])
	{
		forwardSpeed -= 400.0f;
	}
	if (keys[SDL_SCANCODE_A])
	{
		angularSpeed -= Math::Pi;
	}
	if (keys[SDL_SCANCODE_D])
	{
		angularSpeed += Math::Pi;
	}
	// ジャンプ
	if (keys[SDL_SCANCODE_SPACE])
	{
		Jump();
	}
	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);

	/* カメラの設定 */
	// スピードによって水平距離を変えてスピード感を出す
	if (!Math::NearZero(forwardSpeed))
	{
		mCameraComp->SetHorzDist(500.0f);
	}
	else
	{
		mCameraComp->SetHorzDist(350.0f);
	}
}

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();


}

void FollowActor::FixCollisions()
{
	// ワールド空間のボックスを更新するために、自分の
	// ワールド変換を再計算する必要がある
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	// 床・壁との衝突判定
	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		// このPlaneActorと衝突するか
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			// 全ての差を計算する
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			// dxにはdx1,dx2のうち絶対値が小さい方をセットする
			// dy,dzも同様
			float dx = Math::Abs(dx1) < Math::Abs(dx2) ? dx1 : dx2;
			float dy = Math::Abs(dy1) < Math::Abs(dy2) ? dy1 : dy2;
			float dz = Math::Abs(dz1) < Math::Abs(dz2) ? dz1 : dz2;

			// x,y,zのうち最も差が小さい軸で位置を調整する
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

			// 位置を設定しボックスの成分を更新する
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}

void FollowActor::Jump()
{
	float jumpSpeed = 50.0f;
	Vector3 pos = GetPosition();
	pos.z += jumpSpeed;
	SetPosition(pos);
}
