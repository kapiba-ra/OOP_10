#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <SDL/SDL.h>
#include "Math.h"
#include "SoundEvent.h"

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	class Renderer* GetRenderer() { return mRenderer; }
	class AudioSystem* GetAudioSystem() { return mAudioSystem; }
	class PhysWorld* GetPhysWorld() { return mPhysWorld; }
	class HUD* GetHUD() { return mHUD; }
	class PlayerActor* GetPlayer() { return mPlayerActor; }
	struct WeightedGraph* GetGraph() { return mGraph; }

	// CollisionSystemみたいなクラスを作ってそっちでまとめて処理した方がすっきりするかも
	void AddPlane(class PlaneActor* plane);
	void RemovePlane(class PlaneActor* plane);
	std::vector<class PlaneActor*>& GetPlanes() { return mPlanes; }
	void AddItem(class ItemActor* item);
	void RemoveItem(class ItemActor* item);
	std::vector<class ItemActor*>& GetItems() { return mItems; }



	// 状態によって入力をUIに渡すかゲームワールドに渡すかを変える
	enum GameState
	{
		EMainMenu,
		EGameplay,
		EGameclear,
		EGameover,
		EPaused,
		ELevelUp,
		EQuit,

		EStateNum
	};
	GameState GetState() const { return mGameState; }
	void SetState(GameState state) { mGameState = state; }
	void ChangeState(GameState nextState);

	class Font* GetFont(const std::string& fileName);
	// スタック全体を参照で返す
	const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }
	// 指定のUIScreenをスタックにプッシュする
	void PushUI(class UIScreen* screen);

	void LoadText(const std::string& fileName);
	const std::string& GetText(const std::string& key);

	// 一時的にpublicにしている
	void LoadData();
	void UnloadData();

private:
	// processes in RunLoop()
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	// helper
	void HandleKeyPress(int key);
	void OnEnter(GameState nextState);
	void OnExit(GameState nextState);
	void Reset();

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	std::vector<class UIScreen*> mUIStack;
	std::unordered_map<std::string, class Font*> mFonts;
	std::unordered_map<std::string, std::string> mText;

	class Renderer* mRenderer;
	class AudioSystem* mAudioSystem;
	class PhysWorld* mPhysWorld;
	class InputSystem* mInputSystem;
	class HUD* mHUD;

	Uint32 mTicksCount;
	bool mIsRunning;
	bool mUpdatingActors;

	GameState mGameState;
	SoundEvent mMusicEvent;
	std::vector<class PlaneActor*> mPlanes;
	std::vector<class ItemActor*> mItems;
	class PlayerActor* mPlayerActor;

	// ステージからノードを作りエッジをつなぐ
	void CreateNodes();
	struct WeightedGraph* mGraph;

	float mLastEnemyGen;
};

