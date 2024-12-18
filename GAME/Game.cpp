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

	// SDL_TTF�̏�����
	if (TTF_Init() != 0)
	{
		SDL_Log("Failed to initialize SDL_ttf");
		return false;
	}
	// ����ݒ�
	LoadText("Assets/English.gptext");

	// MainMenu����n�܂�
	new MainMenu(this);

	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::LoadData()
{
	// �ȉ��ō쐬����X�e�[�W���O��ɂȂ��Ă���
	CreateStage();

	// Setup lights
	mRenderer->SetAmbientLight(Vector3(0.7f, 0.7f, 0.7f));
	DirectionalLight& dir = mRenderer->GetDirectionalLight();
	dir.mDirection = Vector3(0.0f, -0.7f, -0.7f);
	dir.mDiffuseColor = Vector3(0.8f, 1.0f, 1.0f);
	dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

	// UI elements
	mHUD = new HUD(this);

	// Music�X�^�[�g
	mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

	// ���Έʒu�}�E�X���[�h��L����
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
	while (SDL_PollEvent(&event)) // �C�x���g�̏�Ԃ��擾
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mGameState = EQuit;
			break;
		case SDL_KEYDOWN:
			// �ŏ��̉�����(0->1�ɂȂ�悤�Ȏ�)�̂ݏ�������
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
		// �}�X�^�[�o�X�̃{�����[����������(0���ŏ���0.1��������)
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Max(0.0f, volume - 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case '=': // �V�t�g�������Ȃ��Ă��ǂ��悤��'+'�łȂ�'='�ɂ��Ă���
	{
		// �}�X�^�[�o�X�̃{�����[�����グ��(1���ő��0.1���グ��)
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Min(1.0f, volume + 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case 'e':
	{
		// ��������炷
		mAudioSystem->PlayEvent("event:/Explosion2D");
		break;
	}
	case 'm':
	{
		// �~���[�W�b�N�C�x���g�̃|�[�Y�E�|�[�Y����
		mMusicEvent.SetPaused(!mMusicEvent.GetPaused()); // true��false��toggle�ɂȂ��Ă�
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
	// ���݂�State����o�鎞�ɍs������
	// UI��ʂ�\������n��State�ł�,��������delete�̏���������
	// ����������UI�������悤�ȏ����͂����Ŏw�肵�Ȃ��Ă���

	// ����nextState�Ɉˑ�����͎̂d�����Ȃ���...
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
		// �����������������󋵂��Ȃ�
		break;
	}
}

void Game::Reset()
{
	// ���SkillSystem��Reset����
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

	// �����z�u
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
		// ����
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
	// �����K�v
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
	// �����K�v
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
	// �ŏI�I�ɂ�肽�����Ƃ�,�ǂݍ��񂾃t�@�C����TextMap���Q�Ƃ���
	// ���̒��g�����āAmText�ɃL�[�ƒl��ǉ����邱��

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
	doc.ParseStream(jsonStr);	// parse(���)
	if (!doc.IsObject())
	{
		SDL_Log("Text file %s is not valid JSON", fileName.c_str());
		return;
	}
	// TextMap�̒��g�S�Ăɂ��ă��[�v���n�܂�
	const rapidjson::Value& actions = doc["TextMap"];
	for (rapidjson::Value::ConstMemberIterator itr = actions.MemberBegin();
		itr != actions.MemberEnd(); ++itr)
	{
		// �Ⴆ�΁Aname��"PauseTitle",value��"PAUSED"�Ƃ������
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
	// ���ɂ���Ȃ�|�C���^��Ԃ��āA�Ȃ��Ȃ烍�[�h����
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
	// �O���t�ɂ͑S�Ẵm�[�h�̏�񂪓���
	// �e�m�[�h�ɂ̓G�b�W�̑��ɁAx,y���W��NodeType�Ƃ����񋓌^�ϐ�����������
	mGraph = new WeightedGraph;

	// �܂��̓O���t�Ƀm�[�h��ǉ����āAx,y���W��ݒ肷��B
	// 10�~10��100��
	// �m�[�h�̐ݒ菇�Ԃɍ���̏����͈ˑ����Ă���
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			// -2500.0f�̓X�e�[�W�̐����`(5000�~5000)�̔���
			float centerX = -2500.0f + (i * 500.0f) + 250.0f;
			float centerY = -2500.0f + (j * 500.0f) + 250.0f;

			WeightedGraphNode* node = new WeightedGraphNode();
			node->NodePos.x = centerX;
			node->NodePos.y = centerY;
			mGraph->mNodes.push_back(node);

			// �ǂ̓����̃m�[�h�B�����̓n�[�h�R�[�f�B���O�ŁANodeType��ݒ肷��
			// ���̔���������Ƃ��̂�����ǂ��̂�NodeType����������
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
	// �����܂ł�100�̃m�[�h��xy���W�ENodeType��ݒ肵�I����

	// �m�[�h�̃G�b�W��ݒ肷��
	for (int i = 0; i < 100; ++i)
	{
		WeightedGraphNode* node = mGraph->mNodes[i];
		
		// ���[���h��x���W�͐���,y���W�͉E���ɐL�тĂ邱�Ƃɒ���
		int curNodeX = i / 10;	// �s�C���f�b�N�X
		int curNodeY = i % 10;	// ��C���f�b�N�X
		
		// �O�㍶�E�̃m�[�h���Ȃ��̂ɕ֗�
		// �e�m�[�h�͂��̎l�����̐�Ƀm�[�h�����邩�ǂ����𒲂ׂ邱�ƂɂȂ�
		std::vector<std::pair<int, int>> directions = {
			{1, 0},  // �O
			{-1, 0}, // ��
			{0, -1}, // ��
			{0, 1}   // �E
		};

		for (const auto& dir : directions)
		{
			int neighborX = curNodeX + dir.first;	// �ׂ̃m�[�h�̍s�C���f�b�N�X
			int neighborY = curNodeY + dir.second;	// �ׂ̃m�[�h�̗�C���f�b�N�X
			// �C���f�b�N�X���L���Ȓl�ł��邱�Ƃ��m�F
			if (0 <= neighborX && neighborX < 10 && 0 <= neighborY && neighborY < 10)
			{
				// �ꎟ���ɒ�����
				int neighborIndex = neighborX * 10 + neighborY;
				// �m�[�h���Ƃ��Ă���
				WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];
				// �A�N�Z�X�s�\�łȂ��Ȃ��
				if (neighborNode->type != NodeType::ENoAccess)
				{
					// �G�b�W��ǉ�����
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

	/* 10�~10�~3�O���b�h�̃m�[�h���Q�[�����[���h�ɍ��B */ 
	const int gridZ = 3;
	const int gridX = 10;
	const int gridY = 10;
	const float gridSpacingXY = 500.0f;	// grid�̊Ԋu�B���A�N�^�[�̃T�C�Y�ł�����
	const float gridSpacingZ = 100.0f;	// grid��z�������̍���,�v����
	const int gridNum = gridX * gridY * gridZ;

	/* �܂��̓O���b�h��Ƀm�[�h�����B*/
	for (int i = 0; i < gridZ; ++i)
	{
		for (int j = 0; j < gridX; ++j)
		{
			for (int k = 0; k < gridY; ++k)
			{
				// -2500.0f�̓X�e�[�W�̐����`(5000�~5000)�̔���
				// ����-2500.0f�ɏ��A�N�^�[�̔����̃T�C�Y(250)�𑫂���-2250���o�Ă���
				// 3D��Ԃ̌�둤����A������E�փm�[�h��ǉ����Ă���
				float centerZ = -100.0f + (i * gridSpacingZ);
				float centerX = -2250.0f + (j * gridSpacingXY);
				float centerY = -2250.0f + (k * gridSpacingXY);

				WeightedGraphNode* node = new WeightedGraphNode();
				// �|�W�V������ݒ�
				node->NodePos = Vector3(centerX, centerY, centerZ);
				// �ǉ�(grid��X*Y*Z�̐�����push_back�����)
				mGraph->mNodes.push_back(node);
			}
		}
	}

	/* ���A�N�^�[�̍쐬(�~���l�߂�) */
	for (int i = 0; i < gridX; ++i)
	{
		for (int j = 0; j < gridY; ++j)
		{
			int index = (j * gridX) + i; // 3�����C���f�b�N�X�v�Z(�������Ȃ̂�0-99�܂�)
			WeightedGraphNode* node = mGraph->mNodes[index];
			if (!node->Active)
			{
				node->Active = true;

				Actor* floor = new FloorActor(this);
				// �m�[�h�̏��->Actor�̏�,���̈ʒu���m�[�h�̏ꏊ�Ɉˑ�����
				floor->SetPosition(node->NodePos);
			}
		}
	}
	
	/* �󒆂ɑ���Actor��ݒu���Ă݂� */
	CreateScaffold(111);
	CreateScaffold(112);
	CreateScaffold(213);
	CreateScaffold(118);
	CreateScaffold(134);

	/* �G�b�W���q�� */
	// �����Ƃ��āA�m�[�h���Ȃ��̂ɕ֗���tuple��p�ӂ���
	// �e�m�[�h�͂���6�����̐�ɃA�N�e�B�u�ȃm�[�h�����邩�ǂ����𒲂ׂ邱�ƂɂȂ�
	std::vector<std::tuple<int, int, int>> directions = {
		{1, 0, 0},		// x+1 �O
		{-1, 0, 0},		// x-1 ��
		{0, -1, 0},		// y-1 ��
		{0, 1, 0},		// y+1 �E
		{1, 1, 0},		// x+1,y+1 (�E�O)
		{1, -1, 0},		// x+1,y-1 (���O)
		{-1, 1, 0},		// x-1,y+1 (�E��)
		{-1, -1, 0},	// x-1,y-1 (����)
		//{0,0,1},
		//{0,0,-1}
		{1, 0, 1},		// x+1,z+1 (�O��)
		{1, 0, -1},		// x+1,z-1 (�O��)
		{-1, 0, 1},		// x-1,z+1 (���)
		{-1, 0, -1},	// x-1,z-1 (�㉺)
		{0, 1, 1},		// y+1,z+1 (�E��)
		{0, 1, -1},		// y+1,z-1 (�E��)
		{0, -1, 1},		// y-1,z+1 (����)
		{0, -1, -1},	// y-1,z-1 (����)
	};
	// �G�b�W�q��,�S�m�[�h�����[�v
	for (size_t i = 0; i < mGraph->mNodes.size(); ++i)
	{
		WeightedGraphNode* node = mGraph->mNodes[i];

		// Active�ȃm�[�h�����𒲂ׂ�
		if (!node->Active) continue;

		// ���[���h��x���W�͐���,y���W�͉E���ɐL�тĂ邱�Ƃɒ���
		int curNodeX = (i % (gridX * gridY)) / gridY;	// X�������C���f�b�N�X
		int curNodeY = (i % (gridX * gridY)) % gridY;	// Y�������C���f�b�N�X
		int curNodeZ = i / (gridX * gridY);				// Z�������C���f�b�N�X

		// ���ׂĂ̕����𒲂ׂ�
		for (const auto& dir : directions)
		{
			// direction��tuple���,�אڃm�[�h����index�𓾂�
			int idxX = curNodeX + std::get<0>(dir);
			int idxY = curNodeY + std::get<1>(dir);
			int idxZ = curNodeZ + std::get<2>(dir);

			// �אڃm�[�h���O���b�h���Ɏ��܂邩�m�F
			if (0 <= idxX && idxX < gridX &&
				0 <= idxY && idxY < gridY &&
				0 <= idxZ && idxZ < gridZ)
			{
				// �אڃm�[�h�̃C���f�b�N�X��,�����v�Z�����
				int neighborIndex = (idxZ * gridX * gridY) + (idxX * gridX) + idxY;
				WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];

				// 1.�V��ɑ��ꂪ�����Ēʂ�Ȃ��󋵂��ł��Ă��܂��̂ŁA���̑΍�̏���
				int checkIndex = 0;
				if (idxZ > curNodeZ)
				{
					// �אڂ���Ȃ�A������2��̃m�[�h��Active�Ȃ�A�G�b�W���q���Ȃ�
					checkIndex = i + 2 * gridX * gridY;
					// �L����Index�ŁA����Active�Ȃ��
					if ((checkIndex < gridNum) && (mGraph->mNodes[checkIndex]->Active))
					{
						continue;	// �G�b�W���q���Ȃ�
					}
				}
				else if (idxZ < curNodeZ)
				{
					// �אڂ����Ȃ�A�אڂ�2�オActive�Ȃ�A���Ƃ̃G�b�W���q���Ȃ�
					checkIndex = neighborIndex - 2 * gridX * gridY;
					// �L����Index�ŁA����Active�Ȃ��
					if ((checkIndex >= 0) && (mGraph->mNodes[checkIndex]->Active))
					{
						continue;	// �G�b�W���q���Ȃ�
					}
				}

				// 2.�אڃm�[�h��Active�Ȃ�G�b�W��ǉ�
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
	/* �󒆂ɑ���Actor��ݒu���Ă݂� */
	// �����̓C���f�b�N�X���w�肷�邾���݂����ȏ�Ԃɂ���������

	if (index < mGraph->mNodes.size())
	{
		// ���ꂪ�u�����ꏊ�ɑΉ�����m�[�h��active�ɂ���
		mGraph->mNodes[index]->Active = true;
		// ����ɁA���̎��ӂ̃m�[�h��Active�ɂ���(�ȉ��͑S�����̏���)
		// (�N���s�\�n������ۂ͂��̊֐����Ăяo������ł���K�v������Ǝv��)
		//std::vector<std::tuple<int, int, int>> directions = {
		//	{1, 0, 0},		// x+1 �O
		//	{-1, 0, 0},		// x-1 ��
		//	{0, -1, 0},		// y-1 ��
		//	{0, 1, 0},		// y+1 �E
		//	{1, 1, 0},		// x+1,y+1 (�E�O)
		//	{1, -1, 0},		// x+1,y-1 (���O)
		//	{-1, 1, 0},		// x-1,y+1 (�E��)
		//	{-1, -1, 0},	// x-1,y-1 (����)
		//};
		//int curNodeX = (index % (10 * 10)) / 10;
		//int curNodeY = (index % (10 * 10)) % 10;
		//int curNodeZ = index / (10 * 10);
		//for (const auto& dir : directions)
		//{
		//	// direction��tuple���,�אڃm�[�h����index�𓾂�
		//	int idxX = curNodeX + std::get<0>(dir);
		//	int idxY = curNodeY + std::get<1>(dir);
		//	int idxZ = curNodeZ + std::get<2>(dir);

		//	// �אڃm�[�h���O���b�h���Ɏ��܂邩�m�F
		//	if (0 <= idxX && idxX < 10 &&
		//		0 <= idxY && idxY < 10 &&
		//		0 <= idxZ && idxZ < 3)
		//	{
		//		// �אڃm�[�h�̃C���f�b�N�X��,�����v�Z�����
		//		int neighborIndex = (idxZ * 10 * 10) + (idxY * 10) + idxX;
		//		WeightedGraphNode* neighborNode = mGraph->mNodes[neighborIndex];
		//		mGraph->mNodes[neighborIndex]->Active = true;
		//	}
		//}
		
		// 1.�����Ȃ�ꍇ�ł�,����̐^�����A�N�e�B�u�ȃm�[�h�ɂ���
		int nodeU1idx = index - 100;	// 1���̃m�[�h
		if (nodeU1idx > 0)
		{
			mGraph->mNodes[nodeU1idx]->Active = false;
		}

		// Actor�쐬
		Actor* scaffold = new ScaffoldActor(this);
		scaffold->SetPosition(mGraph->mNodes[index]->NodePos);
	}
}

void Game::CreateStage()
{
	CreateNodes2();

	Actor* actor = nullptr;
	Quaternion q;

	// Stage�̌`�����镔���ł���,�n�[�h�R�[�f�B���O�ɂȂ��Ă���,�o�C�i���f�[�^��
	// ����悤�ɂȂ�Ώ����̕ω��̍ۂɍăR���p�C�����������Ȃ��̂Ŋy(�Q�l��14��)

	const float start = -2250.0f;	// �ŏ��̃A�N�^�[��(-2000,-2000,-100)�̈ʒu�ɍ��
	const float size = 500.0f;		// ��Actor�͏c��500�ł���

	/* ��, ������]�̂��̂���C�ɍ���Ă��� */
	// X��(�O��)��-90�x��]
	q = Quaternion(Vector3::UnitX, -Math::PiOver2);
	for (int i = 0; i < 10; i++)
	{
		// ��
		actor = new WallActor(this);
		actor->SetPosition(Vector3(start + i * size, 2500.0f, 150.0f));// 150�͏��̍���+size/2
		actor->SetRotation(q);
		// �E
		actor = new WallActor(this);
		actor->SetPosition(Vector3(start + i * size, -2500.0f, 150.0f));
		actor->SetRotation(q);
	}
	// X����90�x��]���������̂��X��Z��(�����)��-90�x��]
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, -Math::PiOver2)); 
	for (int i = 0; i < 10; i++)
	{
		// ��
		actor = new WallActor(this);
		actor->SetPosition(Vector3(-2500.0f, start + i * size, 150.0f));
		actor->SetRotation(q);
		// �O
		actor = new WallActor(this);
		actor->SetPosition(Vector3(2500.0f, start + i * size, 150.0f));
		actor->SetRotation(q);
	}
}
