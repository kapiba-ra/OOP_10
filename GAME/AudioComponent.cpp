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

	// mEvent2D,mEvent3D�̖����ɂȂ����C�x���g���폜����
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
	// ���L�A�N�^�[�����[���h�s����v�Z����x�ɌĂяo�����
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
	// 2D��,3D��
	if (event.Is3D())
	{
		mEvents3D.emplace_back(event);
		// 3D����������������
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
