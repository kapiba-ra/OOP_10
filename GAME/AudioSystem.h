#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
#include "Math.h"

namespace FMOD
{
	class System;	// Low Level API�̕�
	namespace Studio
	{
		class System;
		class Bank;
		class EventDescription;
		class EventInstance;
		class Bus;
	};
};

class AudioSystem
{
public:
	AudioSystem(class Game* game);
	~AudioSystem();

	bool Initialize();
	void Shutdown();
	void Update(float deltaTime);

	void LoadBank(const std::string& name);
	void UnLoadBank(const std::string& name);
	void UnloadAllBanks();

	SoundEvent PlayEvent(const std::string& name);

	void SetListener(const Matrix4& viewMatrix);

	// �o�X�̐���
	float GetBusVolume(const std::string& name) const;
	bool GetBusPaused(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	void SetBusPaused(const std::string& name, bool pause);

protected:
	friend class SoundEvent;
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id);

private:
	class Game* mGame;
	FMOD::Studio::System* mSystem;
	FMOD::System* mLowLevelSystem;
	static unsigned int sNextID;	// �C�x���g�C���X�^���X�p��ID

	/*
	*  mBanks : ���[�h���ꂽ�o���N�̘A�z�z��(�L�[:�o���N�̃t�@�C����)
	*  mEvents : �C�x���g������EventDescription�ւ̘A�z�z��(�L�[:event:/oo �̌`��)
	*  mEventInstances : �C�x���g�C���X�^���X�̊Ǘ��B(�L�[:sNextID�Ŋ��蓖�Ă�)
	*  mBuses : �o�X�̘A�z�z��
	*/
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

/* ----- �C�x���g�Ƃ� ----- */
// �Q�[�����炷�T�E���h.
// �����̃T�E���h�t�@�C���E�p�����[�^�E�^�C�~���O��񂪊��蓖�Ă���.
// �T�E���h�t�@�C���𒼐ڍĐ�����̂ł͂Ȃ�,�C�x���g���Đ�����.
// FMOD�̃C�x���g�́AEventDescription�N���X�EEventInstance�N���X�ɕ�����Ă���B
// �O�҂��^�A��҂͂��̌^�̃C���X�^���X�ƍl����B

/* ----- �o���N�Ƃ� ----- */
// �C�x���g�E�T���v���f�[�^�E�X�g���[�~���O�f�[�^������R���e�i�̂���.
// �T���v���f�[�^�͐��̃I�[�f�B�I�f�[�^�ł���A�C�x���g���Q�Ƃ���.

/* ----- �o�X�Ƃ� ----- */
// �T�E���h�̃O���[�v�̂��ƁB
// �����master,SFX,music��3�o�X��p�ӂ��Ă���B(SFX��Sound Effects)