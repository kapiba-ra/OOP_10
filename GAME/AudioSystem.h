#pragma once
#include <unordered_map>
#include <string>
#include "SoundEvent.h"
#include "Math.h"

namespace FMOD
{
	class System;	// Low Level APIの方
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

	// バスの制御
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
	static unsigned int sNextID;	// イベントインスタンス用のID

	/*
	*  mBanks : ロードされたバンクの連想配列(キー:バンクのファイル名)
	*  mEvents : イベント名からEventDescriptionへの連想配列(キー:event:/oo の形式)
	*  mEventInstances : イベントインスタンスの管理。(キー:sNextIDで割り当てる)
	*  mBuses : バスの連想配列
	*/
	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
};

/* ----- イベントとは ----- */
// ゲームが鳴らすサウンド.
// 複数のサウンドファイル・パラメータ・タイミング情報が割り当てられる.
// サウンドファイルを直接再生するのではなく,イベントを再生する.
// FMODのイベントは、EventDescriptionクラス・EventInstanceクラスに分かれている。
// 前者が型、後者はその型のインスタンスと考える。

/* ----- バンクとは ----- */
// イベント・サンプルデータ・ストリーミングデータが入るコンテナのこと.
// サンプルデータは生のオーディオデータであり、イベントが参照する.

/* ----- バスとは ----- */
// サウンドのグループのこと。
// 今回はmaster,SFX,musicの3つバスを用意している。(SFXはSound Effects)