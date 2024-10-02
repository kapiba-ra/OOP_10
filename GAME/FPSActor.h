#pragma once
#include "Actor.h"
#include "SoundEvent.h"

class FPSActor :
    public Actor
{
public:
    FPSActor(class Game* game);

    void UpdateActor(float deltaTime) override;
    void ActorInput(const uint8_t* keys) override;

    void Shoot();

    void SetFootstepSurface(float value);
    void SetVisible(bool visible);
    // 2つのAABBの最小の重なりテストを行う
    void FixCollisions();

private:
    class MoveComponent* mMoveComp;
    class AudioComponent* mAudioComp;
    class MeshComponent* mMeshComp;
    class FPSCamera* mCameraComp;
    class Actor* mFPSModel;
    class BoxComponent* mBoxComp;

    SoundEvent mFootstep;
    float mLastFootstep;
};

