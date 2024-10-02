#include "TargetActor.h"
#include "Game.h"
#include "Renderer.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "TargetComponent.h"

TargetActor::TargetActor(Game* game)
	: Actor(game)
{
	SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Target.gpmesh");
	mc->SetMesh(mesh);
	BoxComponent* bc = new BoxComponent(this);
	bc->SetObjectBox(mesh->GetBox());
	new TargetComponent(this);
}
