#pragma once
#include <vector>
#include <limits>
#include <unordered_map>
#include "Math.h"

struct WeightedEdge
{
	// Edgeにつながっているノード
	struct WeightedGraphNode* mFrom = nullptr;
	struct WeightedGraphNode* mTo = nullptr;
	// Edgeの重み
	// ノードは等間隔においていてedgeの長さも同じなので定数にした
	float mWeight = 1.0f;
};
enum class NodeType
{
	EFloor,
	ENoAccess
};

struct WeightedGraphNode
{
	std::vector<WeightedEdge*> mEdges;
	Vector3 NodePos = Vector3::Zero;	// 3次元位置座標を持たせる
	NodeType type = NodeType::EFloor;
};

struct WeightedGraph
{
	std::vector<WeightedGraphNode*> mNodes;
};

struct AStarScratch
{
	const WeightedEdge* mParentEdge = nullptr;
	float mHeuristic = 0.0f;
	// TODO: ここfloatのmaxにすべきか
	float mActualFromStart = 0.0f;
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};

using AStarMap =
std::unordered_map<const WeightedGraphNode*, AStarScratch>;

float ComputeHeuristic(const WeightedGraphNode* a, const WeightedGraphNode* b);

bool AStarSearch(const WeightedGraph& g, const WeightedGraphNode* start,
	const WeightedGraphNode* goal, AStarMap& outMap);

// 与えられた座標に最も近いノードを取得できる
// もっと効率化できそう
WeightedGraphNode* FindClosestNode(const WeightedGraph& graph, Vector3& pos);

std::vector<const WeightedGraphNode*> ReconstructPath(const WeightedGraphNode* start,
	const WeightedGraphNode* goal, const AStarMap& outMap);

std::vector<Vector3> ReconstructPathCoords(const WeightedGraphNode* start,
	const WeightedGraphNode* goal, const AStarMap& outMap);