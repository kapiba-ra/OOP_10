#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod_studio.hpp>

SoundEvent::SoundEvent()
	: mSystem(nullptr)
	, mID(0)
{
}

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id)
	: mSystem(system)
	, mID(id)
{
}

bool SoundEvent::IsValid()
{
	return (mSystem && mSystem->GetEventInstance(mID) != nullptr);
}

void SoundEvent::Restart()
{
	// ID��nullptr�o�Ȃ����ǂ��������͒��ׂĂ���start()���Ă�(���̊֐���)
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->start();
	}
}

void SoundEvent::Stop(bool allowFadeOut)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
			FMOD_STUDIO_STOP_ALLOWFADEOUT :
			FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

bool SoundEvent::GetPaused() const
{
	bool retVal = false;	// return value(�Ԃ�l)
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPaused(&retVal);
	}
	return retVal;
}

float SoundEvent::GetVolume() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getVolume(&retVal);
	}
	return retVal;
}

float SoundEvent::GetPitch() const
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getPitch(&retVal);
	}
	return retVal;
}

float SoundEvent::GetParameter(const std::string& name)
{
	float retVal = 0.0f;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->getParameterValue(name.c_str(), &retVal);
	}
	return retVal;
}

void SoundEvent::SetPaused(bool pause)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPaused(pause);
	}
}

void SoundEvent::SetVolume(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setVolume(value);
	}
}

void SoundEvent::SetPitch(float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setPitch(value);
	}
}

void SoundEvent::SetParameter(const std::string& name, float value)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		event->setParameterValue(name.c_str(), value);
	}
}

bool SoundEvent::Is3D() const
{
	bool retVal = false;
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		// �C�x���g�L�q�q���擾
		FMOD::Studio::EventDescription* ed = nullptr;
		event->getDescription(&ed);
		if (ed)
		{
			// �C�x���g��3D�Ȃ�true��Ԃ�
			ed->is3D(&retVal);
		}
	}
	return retVal;
}

namespace
{
	// ���C�u�����ƃQ�[�����ʂ̍��W�n���g���Ă���̂ŁA�ϊ��p�̃w���p�[�֐����`����
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		// �Q�[���Ɏg���Ă�����W(�O��,�E,��́A+x, +y, +z)
		// FMOD�̍��W(�O��,�E,��́A+z, +x, +y)
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

void SoundEvent::Set3DAttributes(const Matrix4& worldTrans)
{
	auto event = mSystem ? mSystem->GetEventInstance(mID) : nullptr;
	if (event)
	{
		FMOD_3D_ATTRIBUTES attr;
		// �ʒu�ƕ������Z�b�g����
		attr.position = VecToFMOD(worldTrans.GetTranslation());
		// ���[���h��Ԃł͑�1�s���O���ő�3�s�������
		attr.forward = VecToFMOD(worldTrans.GetXAxis());
		attr.up = VecToFMOD(worldTrans.GetZAxis());
		// ���x�̓[��,(�h�b�v���[���ʂ��g���Ƃ��͏C��)
		attr.velocity = { 0.0f, 0.0f, 0.0f };
		event->set3DAttributes(&attr);
	}
}
