#include "SDL/SDL_scancode.h"
#include "Game.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "MeshComponent.h"
#include "FPSCamera.h"
#include "BoxComponent.h"
#include "FPSActor.h"
#include "PlaneActor.h"
#include "BallActor.h"
#include "Mesh.h"

FPSActor::FPSActor(Game* game)
	: Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);

	mCameraComp = new FPSCamera(this);

	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));

	mBoxComp = new BoxComponent(this);
	// 50×50×175
	AABB myBox(Vector3(-25.0f, -25.0f, -87.5f), Vector3(25.0f, 25.0f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

	mLastFootstep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.0f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	// FPSモデルはアクターとの相対位置により更新
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);
	// アクターの回転で初期化
	Quaternion q = GetRotation();
	// カメラのピッチによる回転
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
}

//void FPSActor::ActorInput(const uint8_t* keys)
//{
//	float forwardSpeed = 0.0f;
//	float strafeSpeed = 0.0f;
//
//	if (keys[SDL_SCANCODE_W])
//	{
//		forwardSpeed += 400.0f;
//	}
//	if (keys[SDL_SCANCODE_S])
//	{
//		forwardSpeed -= 400.0f;
//	}
//	if (keys[SDL_SCANCODE_A])
//	{
//		strafeSpeed -= 400.0f;
//	}
//	if (keys[SDL_SCANCODE_D])
//	{
//		strafeSpeed += 400.0f;
//	}
//
//	mMoveComp->SetForwardSpeed(forwardSpeed);
//	mMoveComp->SetStrafeSpeed(strafeSpeed);
//
//	/* マウスによる角速度の計算 */
//	// SDLから相対運動を取得
//	int x, y;
//	Uint32 buttons = SDL_GetRelativeMouseState(&x, &y);
//	const int maxMouseSpeed = 500;
//	const float maxAngularSpeed = Math::Pi * 8;
//	float angularSpeed = 0.0f;
//	if (x != 0)
//	{
//		// およそ[-1.0, 1.0]の範囲に変換
//		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
//		// 最大移動量での角速度をかける
//		angularSpeed *= maxAngularSpeed;
//	}
//	mMoveComp->SetAngularSpeed(angularSpeed);
//
//	// ピッチの計算
//	const float maxPitchSpeed = Math::Pi * 8;
//	float pitchSpeed = 0.0f;
//	if (y != 0)
//	{
//		// およそ[-1.0, 1.0]の範囲に変換
//		pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
//		// 最大移動量での角速度をかける
//		pitchSpeed *= maxPitchSpeed;
//	}
//	mCameraComp->SetPitchSpeed(pitchSpeed);
//}

void FPSActor::Shoot()
{
	/* 球の位置、方向を決める 関数化した */
	//Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	//Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	//screenPoint.z = 0.9f;
	//Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	//Vector3 dir = end - start;
	//dir.Normalize();
	Vector3 start, dir;
	GetGame()->GetRenderer()->GetScreenDirection(start, dir);

	// 球を作成して色々設定
	BallActor* ball = new BallActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.0f);
	ball->RotateToNewForward(dir);
	// サウンド
	mAudioComp->PlayEvent("event:/Shot");
}

void FPSActor::SetFootstepSurface(float value)
{
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}

void FPSActor::SetVisible(bool visible)
{
	mMeshComp->SetVisible(visible);
}

void FPSActor::FixCollisions()
{
	// ワールド空間のボックスを更新するために、自分の
	// ワールド変換を再計算する必要がある
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

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
			if(Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
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
