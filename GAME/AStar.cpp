#include "AStar.h"
#include <algorithm>
#include <cmath>

float ComputeHeuristic(const WeightedGraphNode* a, const WeightedGraphNode* b)
{
	// マンハッタン距離
	//return std::abs(a->x - b->x) + std::abs(a->y - b->y);
	Vector3 retVec = a->NodePos - b->NodePos;
	
	// ユークリッド距離
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
		// currentノードの全てのエッジについて調べる
		for (const WeightedEdge* edge : current->mEdges)
		{
			// edge->mToで、エッジの繋がり先のノードを示すポインタを得る
			const WeightedGraphNode* neighbor = edge->mTo;
			// Get scratch data for this (neighborのこと) node
			// 新しいキーにアクセスした場合はそのキーと値(ScratchData)が追加される
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
	WeightedGraphNode* closestNode = nullptr; // graph.mNodes[0]とかの方が安全かも
	float closestDist = std::numeric_limits<float>::max(); // floatの最大値

	for (WeightedGraphNode* node : graph.mNodes)
	{
		// Active なノードのみを対象にする
		if (!node->Active) continue;

		Vector3 diff = node->NodePos - pos;

		// Playerの位置との距離を計算
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
		// 現在のノードの親エッジを使って親ノードに遷移
		const AStarScratch& data = outMap.at(current);
		current = data.mParentEdge ? data.mParentEdge->mFrom : nullptr; // mFromノードを取得
		if (!current) break; // 親が存在しない場合は終了
	}

	// 最後にスタートノードを追加
	path.push_back(start);
	std::reverse(path.begin(), path.end()); // 経路を逆順にして正しい順序にする

	return path;
}

std::vector<Vector3> ReconstructPathCoords(const WeightedGraphNode* start, const WeightedGraphNode* goal, const AStarMap& outMap)
{
	std::vector<Vector3> pathCoords;
	const WeightedGraphNode* current = goal;

	while (current != start)
	{
		pathCoords.push_back(current->NodePos);
		// 現在のノードの親エッジを使って親ノードに遷移
		const AStarScratch& data = outMap.at(current);
		current = data.mParentEdge ? data.mParentEdge->mFrom : nullptr; // mFromノードを取得
		if (!current) break; // 親が存在しない場合は終了
	}

	// 最後にスタートノードを追加
	pathCoords.push_back(start->NodePos);
	//std::reverse(pathCoords.begin(), pathCoords.end()); // 経路を逆順にして正しい順序にする

	return pathCoords;
}
