#include "TargetComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"
#include "Math.h"
#include "HUD.h"
#include "BoxComponent.h"

TargetComponent::TargetComponent(Actor* owner)
	: Component(owner)
{
	mOwner->GetGame()->GetHUD()->AddTargetComponent(this);
}

TargetComponent::~TargetComponent()
{
	mOwner->GetGame()->GetHUD()->RemoveTargetComponent(this);
}
