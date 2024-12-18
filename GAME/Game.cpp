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
#include "Skeleton.h"
#include "Animation.h"
#include "AStar.h"

#include "Actor.h"
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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("Failed to initialize InputSystem");
		delete mInputSystem;
		mInputSystem = nullptr;
		return false;
	}

	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Failed to initialize Renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize audio system");
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}

	mPhysWorld = new PhysWorld(this);
	mSkillSystem = new SkillSystem(this);
	mPhaseSystem = new PhaseSystem(this);

	// SDL_TTFの初期化
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}
	// 言語設定
	LoadText("Assets/English.gptext");

	// MainMenuから始まる
	new MainMenu(this);

	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::LoadData()
{
	// 以下で作成するステージが前提になっている
	CreateStage();

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
		//UnloadData();
		OnReturnToMainMenu();
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
	// 先にSkillSystemをResetする
	mSkillSystem->Reset();
	for (auto actor : mActors)
	{
		actor->Reset();
	}

	//for (auto ui : mUIStack)
	//{
	//	ui->Reset();
	//}
	mHUD->Reset();
	mPhaseSystem->Reset();

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
	// 多分必要
	mSkillSystem->Reset();
	mPhaseSystem->Reset();
	
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
	for (auto f : mFonts)
	{
		f.second->Unload();
		delete f.second;
	}
	for (auto s : mSkeletons)
	{
		delete s.second;
	}
}

void Game::OnReturnToMainMenu()
{
	// 多分必要
	mSkillSystem->Reset();
	mPhaseSystem->Reset();

	while (!mActors.empty())
	{
		delete mActors.back();
	}
	while (!mUIStack.empty())
	{
		delete mUIStack.back();
		mUIStack.pop_back();
	}
}

void Game::Shutdown()
{
	UnloadData();
	TTF_Quit();
	delete mSkillSystem;
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
	delete mInputSystem;

	if (mGraph)
	{
		for (WeightedGraphNode* node : mGraph->mNodes)
		{
			for (WeightedEdge* edge : node->mEdges)
			{
				delete edge;
			}
			delete node;
		}
		delete mGraph;
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

void Game::AddEnemy(EnemyActor* enemy)
{
	mEnemies.emplace_back(enemy);
}

void Game::RemoveEnemy(EnemyActor* enemy)
{
	auto iter = std::find(mEnemies.begin(), mEnemies.end(), enemy);
	mEnemies.erase(iter);
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

Skeleton* Game::GetSkeleton(const std::string& fileName)
{
	// 既にあるならポインタを返して、ないならロードする
	auto iter = mSkeletons.find(fileName);
	if (iter != mSkeletons.end())
	{
		return iter->second;
	}
	else
	{
		Skeleton* sk = new Skeleton();
		if (sk->Load(fileName))
		{
			mSkeletons.emplace(fileName, sk);
		}
		else
		{
			delete sk;
			sk = nullptr;
		}
		return sk;
	}
}

Animation* Game::GetAnimation(const std::string& fileName)
{
	auto iter = mAnims.find(fileName);
	if (iter != mAnims.end())
	{
		return iter->second;
	}
	else
	{
		Animation* anim = new Animation();
		if (anim->Load(fileName))
		{
			mAnims.emplace(fileName, anim);
		}
		else
		{
			delete anim;
			anim = nullptr;
		}
		return anim;
	}
}

void Game::CreateNodes()
{
	// グラフには全てのノードの情報が入る
	// 各ノードにはエッジの他に、x,y座標とNodeTypeという列挙型変数を持たせた
	mGraph = new WeightedGraph;

	// まずはグラフにノードを追加して、x,y座標を設定する。
	// 10×10で100回
	// ノードの設定順番に今後の処理は依存している
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			// -2500.0fはステージの正方形(5000×5000)の半分
			float centerX = -2500.0f + (i * 500.0f) + 250.0f;
			float centerY = -2500.0f + (j * 500.0f) + 250.0f;

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
		int curNodeX = i / 10;	// 行インデックス
		int curNodeY = i % 10;	// 列インデックス
		
		// 前後左右のノードをつなぐのに便利
		// 各ノードはこの四方向の先にノードがあるかどうかを調べることになる
		std::vector<std::pair<int, int>> directions = {
			{1, 0},  // 前
			{-1, 0}, // 後
			{0, -1}, // 左
			{0, 1}   // 右
		};

		for (const auto& dir : directions)
		{
			int neighborX = curNodeX + dir.first;	// 隣のノードの行インデックス
			int neighborY = curNodeY + dir.second;	// 隣のノードの列インデックス
			// インデックスが有効な値であることを確認
			if (0 <= neighborX && neighborX < 10 && 0 <= neighborY && neighborY < 10)
			{
				// 一次元に直して
				int neighborIndex = neighborX * 10 + neighborY;
				// ノードをとってくる
				WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];
				// アクセス不可能でないならば
				if (neighborNode->type != NodeType::ENoAccess)
				{
					// エッジを追加する
					WeightedEdge* edge = new WeightedEdge{ node, neighborNode };
					node->mEdges.push_back(edge);
				}
			}
		}
	}
}

void Game::CreateNodes2()
{
	mGraph = new WeightedGraph;

	/* 10×10×3グリッドのノードをゲームワールドに作る。 */ 
	const int gridZ = 3;
	const int gridX = 10;
	const int gridY = 10;
	const float gridSpacingXY = 500.0f;	// gridの間隔。床アクターのサイズでもある
	const float gridSpacingZ = 100.0f;	// gridのz軸方向の高さ,要調節
	const int gridNum = gridX * gridY * gridZ;

	/* まずはグリッド上にノードを作る。*/
	for (int i = 0; i < gridZ; ++i)
	{
		for (int j = 0; j < gridX; ++j)
		{
			for (int k = 0; k < gridY; ++k)
			{
				// -2500.0fはステージの正方形(5000×5000)の半分
				// その-2500.0fに床アクターの半分のサイズ(250)を足して-2250が出てくる
				// 3D空間の後ろ側から、左から右へノードを追加していく
				float centerZ = -100.0f + (i * gridSpacingZ);
				float centerX = -2250.0f + (j * gridSpacingXY);
				float centerY = -2250.0f + (k * gridSpacingXY);

				WeightedGraphNode* node = new WeightedGraphNode();
				// ポジションを設定
				node->NodePos = Vector3(centerX, centerY, centerZ);
				// 追加(gridのX*Y*Zの数だけpush_backされる)
				mGraph->mNodes.push_back(node);
			}
		}
	}

	/* 床アクターの作成(敷き詰める) */
	for (int i = 0; i < gridX; ++i)
	{
		for (int j = 0; j < gridY; ++j)
		{
			int index = (j * gridX) + i; // 3次元インデックス計算(床だけなので0-99まで)
			WeightedGraphNode* node = mGraph->mNodes[index];
			if (!node->Active)
			{
				node->Active = true;

				Actor* floor = new FloorActor(this);
				// ノードの情報->Actorの床,床の位置がノードの場所に依存する
				floor->SetPosition(node->NodePos);
			}
		}
	}
	
	/* 空中に足場Actorを設置してみる */
	CreateScaffold(111);
	CreateScaffold(112);
	CreateScaffold(213);
	CreateScaffold(118);
	CreateScaffold(134);

	/* エッジを繋ぐ */
	// 準備として、ノードをつなぐのに便利なtupleを用意する
	// 各ノードはこの6方向の先にアクティブなノードがあるかどうかを調べることになる
	std::vector<std::tuple<int, int, int>> directions = {
		{1, 0, 0},		// x+1 前
		{-1, 0, 0},		// x-1 後
		{0, -1, 0},		// y-1 左
		{0, 1, 0},		// y+1 右
		{1, 1, 0},		// x+1,y+1 (右前)
		{1, -1, 0},		// x+1,y-1 (左前)
		{-1, 1, 0},		// x-1,y+1 (右後)
		{-1, -1, 0},	// x-1,y-1 (左後)
		//{0,0,1},
		//{0,0,-1}
		{1, 0, 1},		// x+1,z+1 (前上)
		{1, 0, -1},		// x+1,z-1 (前下)
		{-1, 0, 1},		// x-1,z+1 (後上)
		{-1, 0, -1},	// x-1,z-1 (後下)
		{0, 1, 1},		// y+1,z+1 (右上)
		{0, 1, -1},		// y+1,z-1 (右下)
		{0, -1, 1},		// y-1,z+1 (左上)
		{0, -1, -1},	// y-1,z-1 (左下)
	};
	// エッジ繋ぎ,全ノード分ループ
	for (size_t i = 0; i < mGraph->mNodes.size(); ++i)
	{
		WeightedGraphNode* node = mGraph->mNodes[i];

		// Activeなノードだけを調べる
		if (!node->Active) continue;

		// ワールドでx座標は正面,y座標は右側に伸びてることに注意
		int curNodeX = (i % (gridX * gridY)) / gridY;	// X軸方向インデックス
		int curNodeY = (i % (gridX * gridY)) % gridY;	// Y軸方向インデックス
		int curNodeZ = i / (gridX * gridY);				// Z軸方向インデックス

		// すべての方向を調べる
		for (const auto& dir : directions)
		{
			// directionのtupleより,隣接ノード候補のindexを得る
			int idxX = curNodeX + std::get<0>(dir);
			int idxY = curNodeY + std::get<1>(dir);
			int idxZ = curNodeZ + std::get<2>(dir);

			// 隣接ノードがグリッド内に収まるか確認
			if (0 <= idxX && idxX < gridX &&
				0 <= idxY && idxY < gridY &&
				0 <= idxZ && idxZ < gridZ)
			{
				// 隣接ノードのインデックスは,こう計算される
				int neighborIndex = (idxZ * gridX * gridY) + (idxX * gridX) + idxY;
				WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];

				// 1.天井に足場があって通れない状況ができてしまうので、その対策の処理
				int checkIndex = 0;
				if (idxZ > curNodeZ)
				{
					// 隣接が上なら、自分の2つ上のノードがActiveなら、エッジを繋がない
					checkIndex = i + 2 * gridX * gridY;
					// 有効なIndexで、かつActiveならば
					if ((checkIndex < gridNum) && (mGraph->mNodes[checkIndex]->Active))
					{
						continue;	// エッジを繋がない
					}
				}
				else if (idxZ < curNodeZ)
				{
					// 隣接が下なら、隣接の2つ上がActiveなら、下とのエッジを繋がない
					checkIndex = neighborIndex - 2 * gridX * gridY;
					// 有効なIndexで、かつActiveならば
					if ((checkIndex >= 0) && (mGraph->mNodes[checkIndex]->Active))
					{
						continue;	// エッジを繋がない
					}
				}

				// 2.隣接ノードがActiveならエッジを追加
				if (neighborNode->Active)
				{
					WeightedEdge* edge = new WeightedEdge{ node, neighborNode };
					node->mEdges.push_back(edge);
				}
			}
		}
	}
}

void Game::CreateScaffold(size_t index)
{
	/* 空中に足場Actorを設置してみる */
	// ここはインデックスを指定するだけみたいな状態にしたいかも

	if (index < mGraph->mNodes.size())
	{
		// 足場が置かれる場所に対応するノードをactiveにする
		mGraph->mNodes[index]->Active = true;
		// さらに、その周辺のノードをActiveにする(以下は全部その処理)
		// (侵入不可能地域を作る際はこの関数を呼び出した後である必要があると思う)
		//std::vector<std::tuple<int, int, int>> directions = {
		//	{1, 0, 0},		// x+1 前
		//	{-1, 0, 0},		// x-1 後
		//	{0, -1, 0},		// y-1 左
		//	{0, 1, 0},		// y+1 右
		//	{1, 1, 0},		// x+1,y+1 (右前)
		//	{1, -1, 0},		// x+1,y-1 (左前)
		//	{-1, 1, 0},		// x-1,y+1 (右後)
		//	{-1, -1, 0},	// x-1,y-1 (左後)
		//};
		//int curNodeX = (index % (10 * 10)) / 10;
		//int curNodeY = (index % (10 * 10)) % 10;
		//int curNodeZ = index / (10 * 10);
		//for (const auto& dir : directions)
		//{
		//	// directionのtupleより,隣接ノード候補のindexを得る
		//	int idxX = curNodeX + std::get<0>(dir);
		//	int idxY = curNodeY + std::get<1>(dir);
		//	int idxZ = curNodeZ + std::get<2>(dir);

		//	// 隣接ノードがグリッド内に収まるか確認
		//	if (0 <= idxX && idxX < 10 &&
		//		0 <= idxY && idxY < 10 &&
		//		0 <= idxZ && idxZ < 3)
		//	{
		//		// 隣接ノードのインデックスは,こう計算される
		//		int neighborIndex = (idxZ * 10 * 10) + (idxY * 10) + idxX;
		//		WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];
		//		mGraph->mNodes[neighborIndex]->Active = true;
		//	}
		//}
		
		// 1.いかなる場合でも,足場の真下を非アクティブなノードにする
		int nodeU1idx = index - 100;	// 1つ下のノード
		if (nodeU1idx > 0)
		{
			mGraph->mNodes[nodeU1idx]->Active = false;
		}

		// Actor作成
		Actor* scaffold = new ScaffoldActor(this);
		scaffold->SetPosition(mGraph->mNodes[index]->NodePos);
	}
}

void Game::CreateStage()
{
	CreateNodes2();

	Actor* actor = nullptr;
	Quaternion q;

	// Stageの形状を作る部分であり,ハードコーディングになっている,バイナリデータで
	// 作れるようになれば少しの変化の際に再コンパイルが発生しないので楽(参考書14章)

	const float start = -2250.0f;	// 最初のアクターは(-2000,-2000,-100)の位置に作る
	const float size = 500.0f;		// 床Actorは縦横500である

	/* 壁, 同じ回転のものを一気に作っている */
	// X軸(前方)で-90度回転
	q = Quaternion(Vector3::UnitX, -Math::PiOver2);
	for (int i = 0; i < 10; i++)
	{
		// 左
		actor = new WallActor(this);
		actor->SetPosition(Vector3(start + i * size, 2500.0f, 150.0f));// 150は床の高さ+size/2
		actor->SetRotation(q);
		// 右
		actor = new WallActor(this);
		actor->SetPosition(Vector3(start + i * size, -2500.0f, 150.0f));
		actor->SetRotation(q);
	}
	// X軸で90度回転させたものを更にZ軸(上方向)で-90度回転
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, -Math::PiOver2)); 
	for (int i = 0; i < 10; i++)
	{
		// 後
		actor = new WallActor(this);
		actor->SetPosition(Vector3(-2500.0f, start + i * size, 150.0f));
		actor->SetRotation(q);
		// 前
		actor = new WallActor(this);
		actor->SetPosition(Vector3(2500.0f, start + i * size, 150.0f));
		actor->SetRotation(q);
	}
}
