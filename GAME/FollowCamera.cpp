#include "FollowCamera.h"
#include "Actor.h"
#include "PlaneActor.h"
#include "Collision.h"
#include "Game.h"
#include "BoxComponent.h"

FollowCamera::FollowCamera(Actor* owner)
	: CameraComponent(owner)
	, mCameraState(CameraState::EBack)
	, mHorzDist(350.0f)
	, mVertDist(150.0f)
	, mTargetDist(100.0f)
	, mSpringConstant(64.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// ばね定数から減衰を計算
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);

	// 理想の位置を計算
	Vector3 idealPos = ComputeCameraPos();

	// 実際と理想の差を計算
	Vector3 diff = mActualPos - idealPos;
	// ばねによる加速度を計算
	Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;

	// 速度の更新
	mVelocity += acel * deltaTime;
	// 実際のカメラポジションを更新
	mActualPos += mVelocity * deltaTime;
	// 障害物をチェックし、交差していたらカメラの位置を交点に更新
	CheckObstacles();
	// ターゲットは所有アクターから前方に離れた座標
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	// 理想ではなく実際のポジションを使う
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetViewMatrix(view);
}

void FollowCamera::SnapToIdeal()
{
	// 実際の位置は理想の位置と同じ
	mActualPos = ComputeCameraPos();
	mVelocity = Vector3::Zero;
	// 注視点とビューを計算
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetViewMatrix(view);
}

void FollowCamera::SwitchCameraPos()
{
	switch (mCameraState)
	{
	case EAbove:
	{
		mHorzDist = 350.0f;
		mVertDist = 150.0f;
		mCameraState = EBack;
		break;
	}
	case EBack:
	{
		mHorzDist = 100.0f;
		mVertDist = 1000.0f;
		mCameraState = EAbove;
		break;
	}
	}
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// カメラはアクターの上後方にあるようにする
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}

void FollowCamera::CheckObstacles()
{
	LineSegment line(mActualPos, mOwner->GetPosition() + Vector3(0.0f, 0.0f, 50.0f));
	auto& planes = mOwner->GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		//if (pa->GetType() == Actor::EWall)
		//{
		float t = 0.0f;
		Vector3 norm(Vector3::Zero);
		const AABB& planeBox = pa->GetBox()->GetWorldBox();

		if (Intersect(line, planeBox, t, norm))
		{
			mActualPos = line.PointOnSegment(t);
		}
		//}
	}
}
