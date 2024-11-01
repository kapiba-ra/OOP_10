#include "PlaneActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxComponent.h"

PlaneActor::PlaneActor(Game* game)
	: Actor(game, Type::Eplane)
{
	SetScale(10.0f); // 縦横1000の長さになってるはず
	
	/* MeshComponent */
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Plane.gpmesh");
	mc->SetMesh(mesh);
	/* BoxComponent */
	mBox = new BoxComponent(this);
	mBox->SetObjectBox(mesh->GetBox());

	game->AddPlane(this);
}

PlaneActor::~PlaneActor()
{
	GetGame()->RemovePlane(this);
}
