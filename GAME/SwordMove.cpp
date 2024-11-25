#include "SwordMove.h"
#include "Game.h"
#include "PhysWorld.h"
#include "SwordActor.h"
//#include "PlayerActor.h"

SwordMove::SwordMove(Actor* owner)
    : MoveComponent(owner)
{
	// �����̈ʒu�𒆐S�ɉ��悤�ɏ�����
	mPivot = mOwner->GetPosition();
    mOffset = Vector3::Zero;
}

void SwordMove::Update(float deltaTime)
{
    float angle = GetAngularSpeed() * deltaTime;
    Quaternion inc(Vector3::UnitZ, angle);

    mOffset = Vector3::Transform(mOffset, inc);
    Vector3 newPos = mPivot + mOffset;
    mOwner->SetPosition(newPos);

    Quaternion rota = mOwner->GetRotation();
    rota = Quaternion::Concatenate(rota, inc);
    
    mOwner->SetRotation(rota);

    // �A�N�^�[���g�̉�]
    //MoveComponent::Update(deltaTime);
}
