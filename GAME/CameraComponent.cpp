#include "CameraComponent.h"
#include "Game.h"
#include "Actor.h"
#include "Renderer.h"
#include "AudioSystem.h"

CameraComponent::CameraComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{
}

void CameraComponent::SetViewMatrix(const Matrix4& view)
{
	// ƒrƒ…[s—ñ‚ğRenderer,AudioSystem‚É“n‚·
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
	game->GetAudioSystem()->SetListener(view);
}
