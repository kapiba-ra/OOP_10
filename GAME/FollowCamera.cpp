#include "FollowCamera.h"
#include "Actor.h"

FollowCamera::FollowCamera(Actor* owner)
	: CameraComponent(owner)
	//, mHorzDist(350.0f)
	//, mVertDist(150.0f)
	, mHorzDist(100.0f)
	, mVertDist(1000.0f)
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

Vector3 FollowCamera::ComputeCameraPos() const
{
	// カメラはアクターの上後方にあるようにする
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}
