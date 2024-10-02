#pragma once
#include <string>
#include "Math.h"

/* ������ID���g���ăC�x���g�C���X�^���X���Ǘ����邽�߂̃N���X */

class SoundEvent
{
public:
	SoundEvent();

	bool IsValid();	// �Ή�����FMOD�C�x���g�C���X�^���X�������true��Ԃ�
	void Restart();
	void Stop(bool allowFadeOut = true);

	/* ---------- Getters ---------- */
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name);

	/* ---------- Setters ---------- */
	// EventInstance�̋@�\�̃��b�p�[�Ƃ��Ďg��
	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);

	bool Is3D() const;
	void Set3DAttributes(const Matrix4& worldTrans);

protected:
	// AudioSystem���������̃R���X�g���N�^�ɃA�N�Z�X�ł���̂�
	// AudioSystem������SoundEvent��ID�����蓖�Ă邱�Ƃ��ł���
	friend class AudioSystem;
	SoundEvent(class AudioSystem* system, unsigned int id);

private:
	class AudioSystem* mSystem;
	unsigned int mID;
};

// GetEventInstance���Ăяo����EventInstance�|�C���^���擾���A
// EventInstance�̊֐����Ăяo���Ƃ������ꂪ�֐��̍ŏ��Ɏg��ꂪ��