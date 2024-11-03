#include "ItemActor.h"
#include "Game.h"

ItemActor::ItemActor(Game* game)
	: Actor(game, Type::Eitem)
	, mBoxComp(nullptr)
	, mMoveComp(nullptr)
{
	game->AddItem(this);
}

ItemActor::~ItemActor()
{
	GetGame()->RemoveItem(this);
}

void ItemActor::Reset()
{
	SetState(EDead);
}

void ItemActor::OnAcquired()
{
	SetState(EDead);
}
