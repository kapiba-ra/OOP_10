#pragma once
#include "Component.h"
#include <vector>
#include "SoundEvent.h"

/* サウンドイベントを特定のアクターに割り当てる為のクラス */

class AudioComponent :
    public Component
{
public:
    AudioComponent(Actor* owner, int updateOrder = 200);
    ~AudioComponent();

    void Update(float deltaTime) override;
    void OnUpdateWorldTransform() override;

    SoundEvent PlayEvent(const std::string& name);
    void StopAllEvents();

private:
    std::vector<SoundEvent> mEvents2D;
    std::vector<SoundEvent> mEvents3D;
};
