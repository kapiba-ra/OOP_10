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
	// SDL�̏�����
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("SDL���������s: %s", SDL_GetError());
		return false;
	}

	// InputSystem�̍쐬�Ə�����
	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("���̓V�X�e�����������s");
		return false;
	}

	// Renderer���쐬���ď�����
	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Renderer���������s");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}

	// AudioSystem���쐬���ď�����
	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize audio system");
		mAudioSystem->Shutdown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}

	// PhysWorld���쐬
	mPhysWorld = new PhysWorld(this);

	mSkillSystem = new SkillSystem(this);
	mPhaseSystem = new PhaseSystem(this);

	// SDL_TTF�̏�����
	if (TTF_Init() != 0)
	{
		SDL_Log(" SDL_ttf���������s");
		return false;
	}

	LoadText("Assets/English.gptext");
	new MainMenu(this);

	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::LoadData()
{
	// �ȉ��ō쐬����X�e�[�W���O��ɂȂ��Ă���
	CreateNodes();

	Actor* actor = nullptr;
	Quaternion q;
	
	/* ----- Create actors ----- */
	// ��(5000�~5000)
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

	// ��
	q = Quaternion(Vector3::UnitX, Math::PiOver2); // X���͑O���ł��邱�Ƃɒ���
	for (int i = 0; i < 5; i++)
	{
		// ��
		//actor = new PlaneActor(this);
		//actor->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		//actor->SetRotation(q);
		// �E
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(start + i * size, -2500.0f, 0.0f));
		actor->SetRotation(q);
	}
	// �X�e�[�W��̒ʂ�Ȃ��Ƃ���(��)
	// TODO: �ǎl���łȂ��I�u�W�F�N�g�P�ɂ�����
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(1500.0f, 0.0f, 0.0f));
	actor->SetRotation(q);
	actor = new PlaneActor(this);
	actor->SetPosition(Vector3(1500.0f, 1000.0f, 0.0f));
	actor->SetRotation(q);

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	for (int i = 0; i < 5; i++)
	{
		// ��
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(-2500.0f, start + i * size, 0.0f));
		actor->SetRotation(q);
		// �O
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
	for (auto actor : mActors)
	{
		actor->Reset();
	}

	//for (auto ui : mUIStack)
	//{
	//	ui->Reset();
	//}
	mHUD->Reset();

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
			// �e�m�[�h�̃G�b�W�����
			for (WeightedEdge* edge : node->mEdges) {
				delete edge; // �G�b�W�̃����������
			}
			delete node; // �m�[�h�̃����������
		}
		delete mGraph; // �O���t���̂̃����������
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

void Game::CreateNodes()
{
	// �O���t�ɂ͑S�Ẵm�[�h�̏�񂪓���
	// �e�m�[�h�ɂ̓G�b�W�̑��ɁAx,y���W��NodeType�Ƃ����񋓌^�ϐ�����������
	mGraph = new WeightedGraph;

	// �܂��̓O���t�Ƀm�[�h��ǉ����āAx,y���W��ݒ肷��B
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			// -2500.0f�̓X�e�[�W�̐����`(5000�~5000)�̔���
			float centerX = -2500.0f + (i * 500.0f) + 250.0f;
			float centerY = -2500.0f + (j * 500.0f) + 250.0f;

			// TODO: delete�ǂ��ł��̂����
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
		int curNodeX = i / 10;
		int curNodeY = i % 10;
		
		// �O�㍶�E�̃m�[�h���Ȃ��̂ɕ֗�
		std::vector<std::pair<int, int>> directions = {
			{1, 0},  // �O
			{-1, 0}, // ��
			{0, -1}, // ��
			{0, 1}   // �E
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
