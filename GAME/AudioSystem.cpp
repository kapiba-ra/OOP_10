#include "AudioSystem.h"
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <SDL/SDL_log.h>
#include <vector>

// �ÓI�����o�ϐ��Ȃ̂ŃN���X�O�ŏ�����
unsigned int AudioSystem::sNextID = 0;

AudioSystem::AudioSystem(Game* game)
	: mGame(game)
	, mSystem(nullptr)
	, mLowLevelSystem(nullptr)
{
}

AudioSystem::~AudioSystem()
{
}

bool AudioSystem::Initialize()
{
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR,	// ���O���b�Z�[�W���`�コ��ݒ�(�G���[�̂�)
		FMOD_DEBUG_MODE_TTY		// stdout�ɏo��
	);

	// FMOD studio�V�X�e���̃C���X�^���X���쐬
	FMOD_RESULT result;
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD system : %s", FMOD_ErrorString(result));
		return false;
	}

	// FMOD�V�X�e��������������
	result = mSystem->initialize(
		512,					 // �ő哯��������
		FMOD_STUDIO_INIT_NORMAL, // FMOD studio API, �f�t�H���g�ݒ�
		FMOD_INIT_NORMAL,		 // FMOD Low Level API, �f�t�H���g�ݒ�
		nullptr					 // �ǉ��̃h���C�o�f�[�^�͎g��Ȃ�
	);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to initialize FMOD system : %s", FMOD_ErrorString(result));
		return false;
	}

	// Low Level�V�X�e���|�C���^���擾���ĕۑ�
	mSystem->getLowLevelSystem(&mLowLevelSystem);

	// �}�X�^�[�o���N�t�@�C���ł���A�K���K�v�Ȃ̂Ń��[�h����
	LoadBank("Assets/Master Bank.strings.bank"); // ���O�ɂ��A�N�Z�X�p
	LoadBank("Assets/Master Bank.bank");

	return true;
}

void AudioSystem::Shutdown()
{
	UnloadAllBanks();
	// FMOD�V�X�e���̃V���b�g�_�E��
	if (mSystem)
	{
		mSystem->release();
	}
}

void AudioSystem::Update(float deltaTime)
{
	// �X�g�b�v�����C�x���g�C���X�^���X��T��
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		// �C�x���g�̏�Ԃ��擾
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			// �C�x���g�̉���Aid���I�����X�g(done)�ɒǉ�
			e->release();
			done.emplace_back(iter.first);
		}
	}
	// �I�������C�x���g�C���X�^���X�͍폜����
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}
	// FMOD�X�V
	mSystem->update();
}

void AudioSystem::LoadBank(const std::string& name)
{
	/* mSystem�I�u�W�F�N�g��LoadBank�֐��������A���b�p�[��ݒ肷�� */
	// ���d�ǂݍ��݂̖h�~
	if (mBanks.find(name) != mBanks.end())
	{
		return;
	}

	// �o���N�̃��[�h
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL,
		&bank
	);
	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		// �o���N��A�z�z��ɒǉ�
		mBanks.emplace(name, bank);
		// �X�g���[�~���O�ȊO�̃T���v���f�[�^��S�ă��[�h
		bank->loadSampleData();
		// ���̃o���N�ɂ���C�x���g�̐����擾
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// �o���N�ɂ���C�x���g�L�q�q�̃��X�g���擾
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[maxPathLength];
			// �C�x���g�̐��������[�v
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// �C�x���g�̃p�X���擾����
				e->getPath(eventName, maxPathLength, nullptr);
				// �C�x���g�A�z�z��ɒǉ�
				mEvents.emplace(eventName, e);
			}
		}
		/* �o�X�̘A�z�z��ւ̒ǉ� */
		// �o���N���̃o�X�̐����擾
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// �o�X�̃��X�g���擾
			std::vector<FMOD::Studio::Bus*> buses(numBuses);
			bank->getBusList(buses.data(), numBuses, &numBuses);
			char busName[512];
			for (int i = 0; i < numBuses; i++)
			{
				FMOD::Studio::Bus* bus = buses[i];
				bus->getPath(busName, 512, nullptr);
				mBuses.emplace(busName, bus);
			}
		}
	}
}

void AudioSystem::UnLoadBank(const std::string& name)
{
	// ���[�h����Ă��邩�ǂ���
	auto iter = mBanks.find(name);
	if (iter == mBanks.end())
	{
		return;
	}

	/* mEvents���N���A���� */
	// �L�[����l(�o���N�ւ̃|�C���^)�̎擾
	FMOD::Studio::Bank* bank = iter->second;
	// �o���N���̃C�x���g�����擾
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	const int maxPathLength = 512;
	if (numEvents > 0)
	{
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		bank->getEventList(events.data(), numEvents, &numEvents);
		char eventName[maxPathLength];
		for (int i = 0; i < numEvents; i++)
		{
			FMOD::Studio::EventDescription* e = events[i];
			e->getPath(eventName, maxPathLength, nullptr);
			auto eventi = mEvents.find(eventName);
			// mEvents���N���A���鏈��
			if (eventi != mEvents.end())
			{
				mEvents.erase(eventi);
			}
		}
	}
	/* mBuses���N���A���� */
	// �o���N���̃o�X�̐����擾����
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0)
	{
		// �o�X�̔z���p�ӂ���
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[maxPathLength];
		for (int i = 0; i < numBuses; i++)
		{
			FMOD::Studio::Bus* bus = buses[i];
			// �o�X�̃p�X���擾
			bus->getPath(busName, maxPathLength, nullptr);
			// �o�X���폜
			auto busi = mBuses.find(busName);
			if (busi != mBuses.end())
			{
				mBuses.erase(busi);
			}
		}
	}

	// �T���v���f�[�^�E�o���N�̃A�����[�h
	bank->unloadSampleData();
	bank->unload();
	// mBanks�̃N���A
	mBanks.erase(iter);
}

void AudioSystem::UnloadAllBanks()
{
	/* �����ʂ�S�Ẵo���N���A�����[�h����֐� */
	for (auto& iter : mBanks)
	{
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	mEvents.clear();
}

SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
	unsigned int retID = 0;		// returnID 
	auto iter = mEvents.find(name);
	if (iter != mEvents.end())
	{
		// �C�x���g�C���X�^���X�̐���
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// �C�x���g�C���X�^���X���J�n
			event->start();
			// ����ID���擾���A�A�z�z��ɒǉ�
			sNextID++;
			retID = sNextID;
			mEventInstances.emplace(retID, event);
		}
	}
	return SoundEvent(this, retID);
}

namespace
{
	FMOD_VECTOR VecToFMOD(const Vector3& in)
	{
		FMOD_VECTOR v;
		v.x = in.y;
		v.y = in.z;
		v.z = in.x;
		return v;
	}
}

void AudioSystem::SetListener(const Matrix4& viewMatrix)
{
	// �x�N�g���𓾂邽�߂Ƀr���[�s��̋t�s����v�Z����
	Matrix4 invView = viewMatrix;
	invView.Invert();
	FMOD_3D_ATTRIBUTES listener;
	// �ʒu�ƕ������Z�b�g����
	listener.position = VecToFMOD(invView.GetTranslation());
	// �t�r���[�s��ł́A��3�s�񂪑O����
	listener.forward = VecToFMOD(invView.GetZAxis());
	// �t�r���[�s��ł́A��2�s�񂪏����
	listener.up = VecToFMOD(invView.GetYAxis());
	// ���x�̓[���ɃZ�b�g����(�h�b�v���[���ʂ��g���Ƃ��͏C������)
	listener.velocity = { 0.0f, 0.0f, 0.0f };
	// FMOD�ɑ���(0�́A���X�i�[��1�l�����Ƃ����Ӗ�)
	mSystem->setListenerAttributes(0, &listener);
}

float AudioSystem::GetBusVolume(const std::string& name) const
{
	float retVal = 0.0f;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getVolume(&retVal);
	}
	return retVal;
}

bool AudioSystem::GetBusPaused(const std::string& name) const
{
	bool retVal = false;
	const auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->getPaused(&retVal);
	}
	return retVal;
}

void AudioSystem::SetBusVolume(const std::string& name, float volume)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setVolume(volume);
	}
}

void AudioSystem::SetBusPaused(const std::string& name, bool pause)
{
	auto iter = mBuses.find(name);
	if (iter != mBuses.end())
	{
		iter->second->setPaused(pause);
	}
}

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id)
{
	// ID��mEventInstances�ɂ���ΑΉ�����EventInstance�̃|�C���^��Ԃ�
	FMOD::Studio::EventInstance* event = nullptr;
	auto iter = mEventInstances.find(id);
	if (iter != mEventInstances.end())
	{
		event = iter->second;
	}
	return event;
}
