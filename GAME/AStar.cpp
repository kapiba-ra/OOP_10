#include "AStar.h"
#include <algorithm>
#include <cmath>

float ComputeHeuristic(const WeightedGraphNode* a, const WeightedGraphNode* b)
{
	// �}���n�b�^������
	//return std::abs(a->x - b->x) + std::abs(a->y - b->y);
	Vector3 retVec = a->NodePos - b->NodePos;
	
	// ���[�N���b�h����
	return retVec.Length();
}

bool AStarSearch(const WeightedGraph& g, const WeightedGraphNode* start,
	const WeightedGraphNode* goal, AStarMap& outMap)
{
	std::vector<const WeightedGraphNode*> openSet;

	// Set current node to start, and mark in closed set
	const WeightedGraphNode* current = start;
	outMap[current].mInClosedSet = true;

	do
	{
		// Add adjacent nodes to open set
		// current�m�[�h�̑S�ẴG�b�W�ɂ��Ē��ׂ�
		for (const WeightedEdge* edge : current->mEdges)
		{
			// edge->mTo�ŁA�G�b�W�̌q�����̃m�[�h�������|�C���^�𓾂�
			const WeightedGraphNode* neighbor = edge->mTo;
			// Get scratch data for this (neighbor�̂���) node
			// �V�����L�[�ɃA�N�Z�X�����ꍇ�͂��̃L�[�ƒl(ScratchData)���ǉ������
			AStarScratch& data = outMap[neighbor];
			// Only check nodes that aren't in the closed set
			if (!data.mInClosedSet)
			{
				if (!data.mInOpenSet)
				{
					// Not in the open set, so parent must be current
					data.mParentEdge = edge;
					data.mHeuristic = ComputeHeuristic(neighbor, goal);
					// Actual cost is the parent's plus cost of traversing edge
					data.mActualFromStart = outMap[current].mActualFromStart +
						edge->mWeight;
					data.mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
				else
				{
					// Compute what new actual cost is if current becomes parent
					float newG = outMap[current].mActualFromStart + edge->mWeight;
					if (newG < data.mActualFromStart)
					{
						// Current should adopt this node
						data.mParentEdge = edge;
						data.mActualFromStart = newG;
					}
				}
			}
		}

		// If open set is empty, all possible paths are exhausted
		if (openSet.empty())
		{
			break;
		}

		// Find lowest cost node in open set
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b) {
				// Calculate f(x) for nodes a/b
				float fOfA = outMap[a].mHeuristic + outMap[a].mActualFromStart;
				float fOfB = outMap[b].mHeuristic + outMap[b].mActualFromStart;
				return fOfA < fOfB;
			});
		// Set to current and move from open to closed
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = true;
		outMap[current].mInClosedSet = true;
	} while (current != goal);

	// Did we find a path?
	return (current == goal) ? true : false;
}

WeightedGraphNode* FindClosestNode(const WeightedGraph& graph, const Vector3& pos)
{
	WeightedGraphNode* closestNode = nullptr; // graph.mNodes[0]�Ƃ��̕������S����
	float closestDist = std::numeric_limits<float>::max(); // float�̍ő�l

	for (WeightedGraphNode* node : graph.mNodes)
	{
		// Active �ȃm�[�h�݂̂�Ώۂɂ���
		if (!node->Active) continue;

		Vector3 diff = node->NodePos - pos;

		// Player�̈ʒu�Ƃ̋������v�Z
 		float dist =
			//std::sqrt((nodeX - playerX) * (nodeX - playerX) + (nodeY - playerY) * (nodeY - playerY));
			diff.Length();

		if (dist < closestDist)
		{
			closestDist = dist;
			closestNode = node;
		}
	}

	return closestNode;
}

std::vector<const WeightedGraphNode*> ReconstructPath(const WeightedGraphNode* start, const WeightedGraphNode* goal, const AStarMap& outMap)
{
	std::vector<const WeightedGraphNode*> path;
	const WeightedGraphNode* current = goal;

	while (current != start)
	{
		path.push_back(current);
		// ���݂̃m�[�h�̐e�G�b�W���g���Đe�m�[�h�ɑJ��
		const AStarScratch& data = outMap.at(current);
		current = data.mParentEdge ? data.mParentEdge->mFrom : nullptr; // mFrom�m�[�h���擾
		if (!current) break; // �e�����݂��Ȃ��ꍇ�͏I��
	}

	// �Ō�ɃX�^�[�g�m�[�h��ǉ�
	path.push_back(start);
	std::reverse(path.begin(), path.end()); // �o�H���t���ɂ��Đ����������ɂ���

	return path;
}

std::vector<Vector3> ReconstructPathCoords(const WeightedGraphNode* start, const WeightedGraphNode* goal, const AStarMap& outMap)
{
	std::vector<Vector3> pathCoords;
	const WeightedGraphNode* current = goal;

	while (current != start)
	{
		pathCoords.push_back(current->NodePos);
		// ���݂̃m�[�h�̐e�G�b�W���g���Đe�m�[�h�ɑJ��
		const AStarScratch& data = outMap.at(current);
		current = data.mParentEdge ? data.mParentEdge->mFrom : nullptr; // mFrom�m�[�h���擾
		if (!current) break; // �e�����݂��Ȃ��ꍇ�͏I��
	}

	// �Ō�ɃX�^�[�g�m�[�h��ǉ�
	pathCoords.push_back(start->NodePos);
	//std::reverse(pathCoords.begin(), pathCoords.end()); // �o�H���t���ɂ��Đ����������ɂ���

	return pathCoords;
}
