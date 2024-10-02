#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"

AudioComponent::AudioComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{
}

AudioComponent::~AudioComponent()
{
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
	Component::Update(deltaTime);

	// mEvent2D,mEvent3Dの無効になったイベントを削除する
	auto iter = mEvents2D.begin();
	while (iter != mEvents2D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents2D.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	iter = mEvents3D.begin();
	while (iter != mEvents3D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents3D.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AudioComponent::OnUpdateWorldTransform()
{
	// 所有アクターがワールド行列を計算する度に呼び出される
	Matrix4 world = mOwner->GetWorldTransform();
	for (auto& event : mEvents3D)
	{
		if (event.IsValid())
		{
			event.Set3DAttributes(world);
		}
	}
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
	SoundEvent event = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	// 2Dか,3Dか
	if (event.Is3D())
	{
		mEvents3D.emplace_back(event);
		// 3D属性を初期化する
		event.Set3DAttributes(mOwner->GetWorldTransform());
	}
	else
	{
		mEvents2D.emplace_back(event);
	}
	return event;
}

void AudioComponent::StopAllEvents()
{
	for (auto& event : mEvents2D)
	{
		event.Stop();
	}
	for (auto& event : mEvents3D)
	{
		event.Stop();
	}
	mEvents2D.clear();
	mEvents3D.clear();
}
