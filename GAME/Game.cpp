#include "Game.h"
#include <algorithm>
#include "Renderer.h"
#include "AudioSystem.h"
#include "PhysWorld.h"
#include "Actor.h"
#include "FPSActor.h"
#include "PlaneActor.h"
#include "BallActor.h"
#include "TargetActor.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "Font.h"
#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>

Game::Game()
	: mRenderer(nullptr)
	, mAudioSystem(nullptr)
	, mPhysWorld(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false)
	, mCrosshair(nullptr)
	, mFPSActor(nullptr)
	, mHUD(nullptr)
	, mGameState(Game::EGameplay)
	, mTicksCount(0)
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

	// SDL_TTF�̏�����
	if (TTF_Init() != 0)
	{
		SDL_Log(" SDL_ttf���������s");
		return false;
	}

	LoadData();
	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::LoadData()
{
	LoadText("Assets/English.gptext");

	Actor* actor = nullptr;
	Quaternion q;
	
	/* ----- Create actors ----- */
	// ��(3250�~3250)
	const float start = -1250.0f;
	const float size = 250.0f;	// �c��1000�~1000�̃A�N�^�[�Ȃ̂Ō��\�d�˂邱�ƂɂȂ�
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			actor = new PlaneActor(this);
			actor->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// ��
	q = Quaternion(Vector3::UnitX, Math::PiOver2); // X���͑O���ł��邱�Ƃɒ���
	for (int i = 0; i < 10; i++)
	{
		// ��
		//actor = new PlaneActor(this);
		//actor->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		//actor->SetRotation(q);
		// �E
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
		actor->SetRotation(q);
	}
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	for (int i = 0; i < 10; i++)
	{
		// ��
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(start - size, start + i * size, 0.0f));
		actor->SetRotation(q);
		// �O
		actor = new PlaneActor(this);
		actor->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
		actor->SetRotation(q);
	}

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

	// �J�����A�N�^�[�͍���1��
	mFPSActor = new FPSActor(this);

	// �^�[�Q�b�g
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(0.0f, -1450.0f, 200.0f));
	actor->SetRotation(Quaternion(Vector3::UnitZ, Math::PiOver2));
	actor = new TargetActor(this);
	actor->SetPosition(Vector3(0.0f, 1450.0f, 200.0f));
	actor->SetRotation(Quaternion(Vector3::UnitZ, -Math::PiOver2));
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

	const Uint8* state = SDL_GetKeyboardState(NULL);
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
		new PauseMenu(this);
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
	case '1':
	{
		// Load English text
		LoadText("Assets/English.gptext");
		break;
	}
	case '2':
	{
		// Load Russian text
		LoadText("Assets/Russian.gptext");
		break;
	}
	case '3':
	{
		// ���{��̃e�L�X�g�����[�h
		LoadText("Assets/Japanese.gptext");
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
	case SDL_BUTTON_LEFT:
	{
		mFPSActor->Shoot();
		break;
	}
	default:
		break;
	}
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
	if (mRenderer)
	{
		mRenderer->Shutdown();
	}
	if (mAudioSystem)
	{
		mAudioSystem->Shutdown();
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
