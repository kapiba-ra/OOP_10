#pragma once
#include <vector>
#include <limits>
#include <unordered_map>
#include "Math.h"

struct WeightedEdge
{
	// Edge�ɂȂ����Ă���m�[�h
	struct WeightedGraphNode* mFrom = nullptr;
	struct WeightedGraphNode* mTo = nullptr;
	// Edge�̏d��
	// �m�[�h�͓��Ԋu�ɂ����Ă���edge�̒����������Ȃ̂Œ萔�ɂ���
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
	Vector3 NodePos = Vector3::Zero;	// 3�����ʒu���W����������
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
	// TODO: ����float��max�ɂ��ׂ���
	float mActualFromStart = 0.0f;
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};

using AStarMap =
std::unordered_map<const WeightedGraphNode*, AStarScratch>;

float ComputeHeuristic(const WeightedGraphNode* a, const WeightedGraphNode* b);

bool AStarSearch(const WeightedGraph& g, const WeightedGraphNode* start,
	const WeightedGraphNode* goal, AStarMap& outMap);

// �^����ꂽ���W�ɍł��߂��m�[�h���擾�ł���
// �����ƌ������ł�����
WeightedGraphNode* FindClosestNode(const WeightedGraph& graph, Vector3& pos);

std::vector<const WeightedGraphNode*> ReconstructPath(const WeightedGraphNode* start,
	const WeightedGraphNode* goal, const AStarMap& outMap);

std::vector<Vector3> ReconstructPathCoords(const WeightedGraphNode* start,
	const WeightedGraphNode* goal, const AStarMap& outMap);