#pragma once
#include <string>
#include "Math.h"

/* 整数のIDを使ってイベントインスタンスを管理するためのクラス */

class SoundEvent
{
public:
	SoundEvent();

	bool IsValid();	// 対応するFMODイベントインスタンスがあればtrueを返す
	void Restart();
	void Stop(bool allowFadeOut = true);

	/* ---------- Getters ---------- */
	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name);

	/* ---------- Setters ---------- */
	// EventInstanceの機能のラッパーとして使う
	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);

	bool Is3D() const;
	void Set3DAttributes(const Matrix4& worldTrans);

protected:
	// AudioSystemだけがこのコンストラクタにアクセスできるので
	// AudioSystemだけがSoundEventにIDを割り当てることができる
	friend class AudioSystem;
	SoundEvent(class AudioSystem* system, unsigned int id);

private:
	class AudioSystem* mSystem;
	unsigned int mID;
};

// GetEventInstanceを呼び出してEventInstanceポインタを取得し、
// EventInstanceの関数を呼び出すという流れが関数の最初に使われがち