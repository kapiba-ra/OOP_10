#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	: CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f) // 60度
	, mPitch(0.0f)
{
}

void FPSCamera::Update(float deltaTime)
{
	/* ピッチなしのカメラ */
	//// カメラの位置は所有アクターの位置
	//Vector3 cameraPos = mOwner->GetPosition();
	//// ターゲットの位置は所有アクターの前方100単位
	//Vector3 target = cameraPos + mOwner->GetForward() * 100.0f;
	//// 上方ベクトルは常にz軸の基本ベクトル
	//Vector3 up = Vector3::UnitZ;
	//// 注視行列を作ってビューに設定する
	//Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	//SetViewMatrix(view);

	/* ピッチあり */
	CameraComponent::Update(deltaTime);

	// カメラの位置は所有アクターの位置
	Vector3 cameraPos = mOwner->GetPosition();
	// ピッチの角速度に基づいてピッチを更新
	mPitch += mPitchSpeed * deltaTime;
	// ピッチの範囲を制限
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// ピッチ回転を表すクォータニオンを作る
	// (オーナーの右向きベクトルを軸とする回転)
	Quaternion q(mOwner->GetRight(), mPitch);
	// ピッチのクォータニオンで所有アクターの前方ベクトルを回転
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	// ターゲットの位置は所有アクターの前方100単位
	Vector3 target = cameraPos + viewForward * 100.0f;
	// 上方ベクトルもピッチのクォータニオンで回転
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// 注視行列を作ってビューに設定
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}
