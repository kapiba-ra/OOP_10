#include "AudioSystem.h"
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <SDL/SDL_log.h>
#include <vector>

// 静的メンバ変数なのでクラス外で初期化
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
		FMOD_DEBUG_LEVEL_ERROR,	// ログメッセージの饒舌さを設定(エラーのみ)
		FMOD_DEBUG_MODE_TTY		// stdoutに出力
	);

	// FMOD studioシステムのインスタンスを作成
	FMOD_RESULT result;
	result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to create FMOD system : %s", FMOD_ErrorString(result));
		return false;
	}

	// FMODシステムを初期化する
	result = mSystem->initialize(
		512,					 // 最大同時発音数
		FMOD_STUDIO_INIT_NORMAL, // FMOD studio API, デフォルト設定
		FMOD_INIT_NORMAL,		 // FMOD Low Level API, デフォルト設定
		nullptr					 // 追加のドライバデータは使わない
	);
	if (result != FMOD_OK)
	{
		SDL_Log("Failed to initialize FMOD system : %s", FMOD_ErrorString(result));
		return false;
	}

	// Low Levelシステムポインタを取得して保存
	mSystem->getLowLevelSystem(&mLowLevelSystem);

	// マスターバンクファイルであり、必ず必要なのでロードする
	LoadBank("Assets/Master Bank.strings.bank"); // 名前によるアクセス用
	LoadBank("Assets/Master Bank.bank");

	return true;
}

void AudioSystem::Shutdown()
{
	UnloadAllBanks();
	// FMODシステムのシャットダウン
	if (mSystem)
	{
		mSystem->release();
	}
}

void AudioSystem::Update(float deltaTime)
{
	// ストップしたイベントインスタンスを探す
	std::vector<unsigned int> done;
	for (auto& iter : mEventInstances)
	{
		FMOD::Studio::EventInstance* e = iter.second;
		// イベントの状態を取得
		FMOD_STUDIO_PLAYBACK_STATE state;
		e->getPlaybackState(&state);
		if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
		{
			// イベントの解放、idを終了リスト(done)に追加
			e->release();
			done.emplace_back(iter.first);
		}
	}
	// 終了したイベントインスタンスは削除する
	for (auto id : done)
	{
		mEventInstances.erase(id);
	}
	// FMOD更新
	mSystem->update();
}

void AudioSystem::LoadBank(const std::string& name)
{
	/* mSystemオブジェクトもLoadBank関数を持つが、ラッパーを設定する */
	// 多重読み込みの防止
	if (mBanks.find(name) != mBanks.end())
	{
		return;
	}

	// バンクのロード
	FMOD::Studio::Bank* bank = nullptr;
	FMOD_RESULT result = mSystem->loadBankFile(
		name.c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL,
		&bank
	);
	const int maxPathLength = 512;
	if (result == FMOD_OK)
	{
		// バンクを連想配列に追加
		mBanks.emplace(name, bank);
		// ストリーミング以外のサンプルデータを全てロード
		bank->loadSampleData();
		// このバンクにあるイベントの数を取得
		int numEvents = 0;
		bank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			// バンクにあるイベント記述子のリストを取得
			std::vector<FMOD::Studio::EventDescription*> events(numEvents);
			bank->getEventList(events.data(), numEvents, &numEvents);
			char eventName[maxPathLength];
			// イベントの数だけループ
			for (int i = 0; i < numEvents; i++)
			{
				FMOD::Studio::EventDescription* e = events[i];
				// イベントのパスを取得して
				e->getPath(eventName, maxPathLength, nullptr);
				// イベント連想配列に追加
				mEvents.emplace(eventName, e);
			}
		}
		/* バスの連想配列への追加 */
		// バンク中のバスの数を取得
		int numBuses = 0;
		bank->getBusCount(&numBuses);
		if (numBuses > 0)
		{
			// バスのリストを取得
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
	// ロードされているかどうか
	auto iter = mBanks.find(name);
	if (iter == mBanks.end())
	{
		return;
	}

	/* mEventsをクリアする */
	// キーから値(バンクへのポインタ)の取得
	FMOD::Studio::Bank* bank = iter->second;
	// バンク内のイベント数を取得
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
			// mEventsをクリアする処理
			if (eventi != mEvents.end())
			{
				mEvents.erase(eventi);
			}
		}
	}
	/* mBusesをクリアする */
	// バンク中のバスの数を取得する
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0)
	{
		// バスの配列を用意する
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[maxPathLength];
		for (int i = 0; i < numBuses; i++)
		{
			FMOD::Studio::Bus* bus = buses[i];
			// バスのパスを取得
			bus->getPath(busName, maxPathLength, nullptr);
			// バスを削除
			auto busi = mBuses.find(busName);
			if (busi != mBuses.end())
			{
				mBuses.erase(busi);
			}
		}
	}

	// サンプルデータ・バンクのアンロード
	bank->unloadSampleData();
	bank->unload();
	// mBanksのクリア
	mBanks.erase(iter);
}

void AudioSystem::UnloadAllBanks()
{
	/* 文字通り全てのバンクをアンロードする関数 */
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
		// イベントインスタンスの生成
		FMOD::Studio::EventInstance* event = nullptr;
		iter->second->createInstance(&event);
		if (event)
		{
			// イベントインスタンスを開始
			event->start();
			// 次のIDを取得し、連想配列に追加
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
	// ベクトルを得るためにビュー行列の逆行列を計算する
	Matrix4 invView = viewMatrix;
	invView.Invert();
	FMOD_3D_ATTRIBUTES listener;
	// 位置と方向をセットする
	listener.position = VecToFMOD(invView.GetTranslation());
	// 逆ビュー行列では、第3行列が前方向
	listener.forward = VecToFMOD(invView.GetZAxis());
	// 逆ビュー行列では、第2行列が上方向
	listener.up = VecToFMOD(invView.GetYAxis());
	// 速度はゼロにセットする(ドップラー効果を使うときは修正する)
	listener.velocity = { 0.0f, 0.0f, 0.0f };
	// FMODに送る(0は、リスナーが1人だけという意味)
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
	// IDがmEventInstancesにあれば対応するEventInstanceのポインタを返す
	FMOD::Studio::EventInstance* event = nullptr;
	auto iter = mEventInstances.find(id);
	if (iter != mEventInstances.end())
	{
		event = iter->second;
	}
	return event;
}
