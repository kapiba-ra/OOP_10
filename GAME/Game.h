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
	class SkillSystem* GetSkillSystem() { return mSkillSystem; }
	class PhaseSystem* GetPhaseSystem() { return mPhaseSystem; }
	class HUD* GetHUD() { return mHUD; }
	class PlayerActor* GetPlayer() { return mPlayerActor; }
	struct WeightedGraph* GetGraph() { return mGraph; }

	// CollisionSystem�݂����ȃN���X������Ă������ł܂Ƃ߂ď������������������肷�邩��
	void AddPlane(class PlaneActor* plane);
	void RemovePlane(class PlaneActor* plane);
	std::vector<class PlaneActor*>& GetPlanes() { return mPlanes; }
	void AddItem(class ItemActor* item);
	void RemoveItem(class ItemActor* item);
	std::vector<class ItemActor*>& GetItems() { return mItems; }



	// ��Ԃɂ���ē��͂�UI�ɓn�����Q�[�����[���h�ɓn������ς���
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
	// �X�^�b�N�S�̂��Q�ƂŕԂ�
	const std::vector<class UIScreen*>& GetUIStack() { return mUIStack; }
	// �w���UIScreen���X�^�b�N�Ƀv�b�V������
	void PushUI(class UIScreen* screen);

	void LoadText(const std::string& fileName);
	const std::string& GetText(const std::string& key);
	class Skeleton* GetSkeleton(const std::string& fileName);
	class Animation* GetAnimation(const std::string& fileName);

	// �ꎞ�I��public�ɂ��Ă���
	void LoadData();
	void UnloadData();
	// �߂����ᖼ�O�͓K��
	void OnReturnToMainMenu();

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
	std::unordered_map<std::string, class Skeleton*> mSkeletons;
	std::unordered_map<std::string, class Animation*> mAnims;

	class Renderer* mRenderer;
	class AudioSystem* mAudioSystem;
	class PhysWorld* mPhysWorld;
	class InputSystem* mInputSystem;
	class SkillSystem* mSkillSystem;
	class PhaseSystem* mPhaseSystem;
	class HUD* mHUD;

	Uint32 mTicksCount;
	bool mIsRunning;
	bool mUpdatingActors;

	GameState mGameState;
	SoundEvent mMusicEvent;
	std::vector<class PlaneActor*> mPlanes;
	std::vector<class ItemActor*> mItems;
	class PlayerActor* mPlayerActor;

	// �X�e�[�W����m�[�h�����G�b�W���Ȃ�
	void CreateNodes();
	struct WeightedGraph* mGraph;

	float mLastEnemyGen;
};

