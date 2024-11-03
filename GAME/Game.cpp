#include "Game.h"
#include <algorithm>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <utility> // for 'pair'
#include <rapidjson/document.h>
#include "Renderer.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "PhysWorld.h"
#include "SkillSystem.h"
#include "PhaseSystem.h"
#include "Font.h"
#include "AStar.h"

#include "Actor.h"
#include "FPSActor.h"
#include "PlayerActor.h"
#include "PlaneActor.h"
#include "BallActor.h"
#include "EnemyActor.h"
#include "HeartActor.h"

#include "MeshComponent.h"
#include "SpriteComponent.h"

#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"
#include "MainMenu.h"
#include "GameOverMenu.h"
#include "GameClearMenu.h"
#include "LevelUpMenu.h"


Game::Game()
	: mRenderer(nullptr)
	, mAudioSystem(nullptr)
	, mPhysWorld(nullptr)
	, mInputSystem(nullptr)
	, mSkillSystem(nullptr)
	, mPhaseSystem(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
	, mPlayerActor(nullptr)
	, mHUD(nullptr)
	, mGameState(Game::EMainMenu)
	, mTicksCount(0)
	, mGraph(nullptr)
	, mLastEnemyGen(0.0f)
{
}

bool Game::Initialize()
{
	// SDLの初期化
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("SDL初期化失敗: %s", SDL_GetError());
		return false;
	}

	// InputSystemの作成と初期化
	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("入力システム初期化失敗");
		return false;
	}

	// Rendererを作成して初期化
	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Renderer初期化失敗");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	// AudioSystemを作成して初期化
	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize audio system");
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}

	// PhysWorldを作成
	mPhysWorld = new PhysWorld(this);

	mSkillSystem = new SkillSystem(this);
	mPhaseSystem = new PhaseSystem(this);

	// SDL_TTFの初期化
	if (TTF_Init() != 0)
	{
		SDL_Log(" SDL_ttf初期化失敗");
		return false;
	}

	LoadText("Assets/English.gptext");
	new MainMenu(this);

	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::LoadData()
{
	// 以下で作成するステージが前提になっている
	CreateNodes();

	Actor* actor = nullptr;
	Quaternion q;
	
	/* ----- Create actors ----- */
	// 床(5000×5000)
	const float start = -2000.0f;
	const float size = 1000.0f;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			actor = new PlaneActor(this);
			actor->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// 壁
	q = Quaternion(Vector3::UnitX, Math::PiOver2); // X軸は前方であることに注意
	for (int i = 0; i < 5; i++)
	{
		// 左
		//actor = new PlaneActor(this);
		//actor->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		//actor->SetRotation(q);
		// 右
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(start + i * size, -2500.0f, 0.0f));
		actor->SetRotation(q);
	}
	// ステージ上の通れないところ(壁)
	// TODO: 壁四枚でなくオブジェクト１個にしたい
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(1500.0f, 0.0f, 0.0f));
	actor->SetRotation(q);
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(1500.0f, 1000.0f, 0.0f));
	actor->SetRotation(q);

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	for (int i = 0; i < 5; i++)
	{
		// 後
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(-2500.0f, start + i * size, 0.0f));
		actor->SetRotation(q);
		// 前
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(2500.0f, start + i * size, 0.0f));
		actor->SetRotation(q);
	}
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(1000.0f, 500.0f, 0.0f));
	actor->SetRotation(q);
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(2000.0f, 500.0f, 0.0f));
	actor->SetRotation(q);

	// Setup lights
	mRenderer->SetAmbientLight(Vector3(0.7f, 0.7f, 0.7f));
	DirectionalLight& dir = mRenderer->GetDirectionalLight();
	dir.mDirection = Vector3(0.0f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3(0.8f, 1.0f, 1.0f);
	dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

	// UI elements
	mHUD = new HUD(this);

	// Musicスタート
	mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

	// 相対位置マウスモードを有効に
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GetRelativeMouseState(nullptr, nullptr);

	mPlayerActor = new PlayerActor(this);

	// Enemy
	mPhaseSystem->StartPhase();
}

void Game::RunLoop()
{
	while (mGameState != EQuit)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	mInputSystem->PrepareForUpdate();
	SDL_Event event;
	while (SDL_PollEvent(&event)) // イベントの状態を取得
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mGameState = EQuit;
			break;
		case SDL_KEYDOWN:
			// 最初の押下時(0->1になるような時)のみ処理する
			if (!event.key.repeat)
			{
				if (mGameState == EGameplay)
				{
					HandleKeyPress(event.key.keysym.sym); // sym...symbol
				}
				else if (!mUIStack.empty())
				{
					mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (mGameState == EGameplay)
			{
				HandleKeyPress(event.button.button);
			}
			else if (!mUIStack.empty())
			{
				mUIStack.back()->HandleKeyPress(event.button.button);
			}
			break;
		default:
			break;
		}
	}

	mInputSystem->Update();

	const InputState& state = mInputSystem->GetState();
	//const Uint8* state = SDL_GetKeyboardState(NULL);
	if (mGameState == EGameplay)
	{
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EActive)
			{
				actor->ProcessInput(state);
			}
		}
	}
	else if (!mUIStack.empty())
	{
		mUIStack.back()->ProcessInput(state);
	}
}

void Game::HandleKeyPress(int key)
{
	switch (key)
	{
	case SDLK_ESCAPE:
	{
		ChangeState(EPaused);
		// new PauseMenu(this);
		break;
	}
	case '-':
	{
		// マスターバスのボリュームを下げる(0が最小で0.1ずつ下げる)
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Max(0.0f, volume - 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case '=': // シフトを押さなくても良いように'+'でなく'='にしている
	{
		// マスターバスのボリュームを上げる(1が最大で0.1ずつ上げる)
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Min(1.0f, volume + 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case 'e':
	{
		// 爆発音を鳴らす
		mAudioSystem->PlayEvent("event:/Explosion2D");
		break;
	}
	case 'm':
	{
		// ミュージックイベントのポーズ・ポーズ解除
		mMusicEvent.SetPaused(!mMusicEvent.GetPaused()); // trueとfalseのtoggleになってる
		break;
	}
	default:
		break;
	}
}

void Game::OnEnter(GameState state)
{
	switch (state)
	{
	case EGameclear:
		new GameClearMenu(this);
		break;
	case EGameover:
		new GameOverMenu(this);
		break;
	case EGameplay:
		// LoadData();
		break;
	case EMainMenu:
		UnloadData();
		new MainMenu(this);
		break;
	case EPaused:
		new PauseMenu(this);
		break;
	case ELevelUp:
		new LevelUpMenu(this);
		break;
	case EQuit:
		break;
	}
}

void Game::OnExit(GameState nextState)
{
	// 現在のStateから出る時に行う処理
	// UI画面を表示する系のStateでは,そっちでdeleteの準備をする
	// したがってUIを消すような処理はここで指定しなくていい

	// 結局nextStateに依存するのは仕方がないか...
	switch (mGameState)
	{
	case EGameclear: // ok
		break;
	case EGameover:
		if (nextState == EGameplay)
		{
			Reset();
		}
		break;
	case EGameplay:
		break;
	case EMainMenu:
		if (nextState == EGameplay)
		{
			LoadData();
		}
		break;
	case EPaused:
		break;
	case ELevelUp:
		break;
	case EQuit:
		// そもそもこういう状況がない
		break;
	}
}

void Game::Reset()
{
	for (auto actor : mActors)
	{
		actor->Reset();
	}

	//for (auto ui : mUIStack)
	//{
	//	ui->Reset();
	//}
	mHUD->Reset();

	// 初期配置
	Actor* actor;
	actor = new EnemyActor(this);
	actor = new EnemyActor(this);
	actor->SetPosition(Vector3(-400.0f, 400.0f, 0.0f));
	actor = new EnemyActor(this);
	actor->SetPosition(Vector3(-300.0f, -300.0f, 0.0f));
	actor = new EnemyActor(this);
	actor->SetPosition(Vector3(300.0f, -300.0f, 0.0f));
	actor = new HeartActor(this);
}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16)) {};

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();
	
	if (mGameState == EGameplay)
	{
		//mLastEnemyGen += deltaTime;
		//if (mLastEnemyGen >= 2.0f)
		//{
		//	mLastEnemyGen -= 2.0f;
		//	new EnemyActor(this);
		//}
		// 諸説
		mPhaseSystem->Update(deltaTime);

		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;

		for (auto pending : mPendingActors)
		{
			pending->ComputeWorldTransform();
			mActors.emplace_back(pending);
		}
		mPendingActors.clear();

		std::vector<Actor*> deadActors;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}

		for (auto actor : deadActors)
		{
			delete actor;
		}
	}

	mAudioSystem->Update(deltaTime);

	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::EActive)
		{
			ui->Update(deltaTime);
		}
	}
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::EClosing)
		{
			delete* iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Game::GenerateOutput()
{
	mRenderer->Draw();
}

void Game::UnloadData()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	while (!mUIStack.empty())
	{
		delete mUIStack.back();
		mUIStack.pop_back();
	}

	if (mRenderer)
	{
		mRenderer->UnloadData();
	}
}

void Game::Shutdown()
{
	UnloadData();
	TTF_Quit();
	delete mPhysWorld;
	if (mRenderer)
	{
		mRenderer->Shutdown();
	}
	if (mAudioSystem)
	{
		mAudioSystem->Shutdown();
	}

	delete mPhaseSystem;
	delete mSkillSystem;
	delete mInputSystem;

	if (mGraph) {
		for (WeightedGraphNode* node : mGraph->mNodes) {
			// 各ノードのエッジを解放
			for (WeightedEdge* edge : node->mEdges) {
				delete edge; // エッジのメモリを解放
			}
			delete node; // ノードのメモリを解放
		}
		delete mGraph; // グラフ自体のメモリを解放
	}

	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
	if (iter != mPendingActors.end())
	{
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddPlane(PlaneActor* plane)
{
	mPlanes.emplace_back(plane);
}

void Game::RemovePlane(PlaneActor* plane)
{
	auto iter = std::find(mPlanes.begin(), mPlanes.end(), plane);
	mPlanes.erase(iter);
}

void Game::AddItem(ItemActor* item)
{
	mItems.emplace_back(item);
}

void Game::RemoveItem(ItemActor* item)
{
	auto iter = std::find(mItems.begin(), mItems.end(), item);
	mItems.erase(iter);
}

void Game::ChangeState(GameState nextState)
{
	OnExit(nextState);
	mGameState = nextState;
	OnEnter(mGameState);
}

Font* Game::GetFont(const std::string& fileName)
{
	auto iter = mFonts.find(fileName);
	if (iter != mFonts.end())
	{
		return iter->second;
	}
	else
	{
		Font* font = new Font(this);
		if (font->Load(fileName))
		{
			mFonts.emplace(fileName, font);
		}
		else
		{
			font->Unload();
			delete font;
			font = nullptr;
		}
		return font;
	}
}

void Game::PushUI(UIScreen* screen)
{
	mUIStack.emplace_back(screen);
}

void Game::LoadText(const std::string& fileName)
{
	// 最終的にやりたいことは,読み込んだファイルのTextMapを参照して
	// その中身を見て、mTextにキーと値を追加すること

	mText.clear();
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("Text file %s not found", fileName.c_str());
		return;
	}
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);	// parse(解析)
	if (!doc.IsObject())
	{
		SDL_Log("Text file %s is not valid JSON", fileName.c_str());
		return;
	}
	// TextMapの中身全てについてループが始まる
	const rapidjson::Value& actions = doc["TextMap"];
	for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
		itr != actions.MemberEnd(); ++itr)
	{
		// 例えば、nameは"PauseTitle",valueは"PAUSED"といった具合
		if (itr->name.IsString() && itr->value.IsString())
		{
			mText.emplace(itr->name.GetString(), itr->value.GetString());
		}
	}
}

const std::string& Game::GetText(const std::string& key)
{
	static std::string errorMsg("**KEY NOT FOUND**");
	auto iter = mText.find(key);
	if (iter != mText.end())
	{
		return iter->second;
	}
	else
	{
		return errorMsg;
	}
}

void Game::CreateNodes()
{
	// グラフには全てのノードの情報が入る
	// 各ノードにはエッジの他に、x,y座標とNodeTypeという列挙型変数を持たせた
	mGraph = new WeightedGraph;

	// まずはグラフにノードを追加して、x,y座標を設定する。
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			// -2500.0fはステージの正方形(5000×5000)の半分
			float centerX = -2500.0f + (i * 500.0f) + 250.0f;
			float centerY = -2500.0f + (j * 500.0f) + 250.0f;

			// TODO: deleteどこでやるのか問題
			WeightedGraphNode* node = new WeightedGraphNode();
			node->NodePos.x = centerX;
			node->NodePos.y = centerY;
			mGraph->mNodes.push_back(node);

			// 壁の内側のノード。ここはハードコーディングで、NodeTypeを設定する
			// この判定をずっとやるのがしんどいのでNodeTypeを持たせた
			if ((centerX == 1250.0f && centerY == 250.0f) ||
				(centerX == 1250.0f && centerY == 750.0f) ||
				(centerX == 1750.0f && centerY == 250.0f) ||
				(centerX == 1750.0f && centerY == 750.0f)
				)
			{
				node->type = NodeType::ENoAccess;
			}
		}
	}
	// ここまでで100個のノードのxy座標・NodeTypeを設定し終えた

	// ノードのエッジを設定する
	for (int i = 0; i < 100; ++i)
	{
		WeightedGraphNode* node = mGraph->mNodes[i];
		
		// ワールドでx座標は正面,y座標は右側に伸びてることに注意
		int curNodeX = i / 10;
		int curNodeY = i % 10;
		
		// 前後左右のノードをつなぐのに便利
		std::vector<std::pair<int, int>> directions = {
			{1, 0},  // 前
			{-1, 0}, // 後
			{0, -1}, // 左
			{0, 1}   // 右
		};

		for (const auto& dir : directions)
		{
			int neighborX = curNodeX + dir.first;
			int neighborY = curNodeY + dir.second;
			
			if (0 <= neighborX && neighborX < 10 && 0 <= neighborY && neighborY < 10)
			{
				int neighborIndex = neighborX * 10 + neighborY;
				WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];
				if (neighborNode->type != NodeType::ENoAccess)
				{
					WeightedEdge* edge = new WeightedEdge{ node, neighborNode };
					node->mEdges.push_back(edge);
				}
			}
		}
	}
}
