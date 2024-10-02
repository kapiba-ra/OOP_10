// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Math.h"
#include <vector>

// 線分
struct LineSegment
{
	LineSegment(const Vector3& start, const Vector3& end);
	// 線分上の点を返す (0 <= t <= 1)
	Vector3 PointOnSegment(float t) const;
	// 点と線分の最短距離の２乗を返す
	float MinDistSq(const Vector3& point) const;
	// Get MinDistSq between two line segments
	static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector3 mStart;	// 始点
	Vector3 mEnd;	// 終点
};

// 平面
struct Plane
{
	Plane(const Vector3& normal, float d);
	// 3つの点から平面を構築する
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	// 任意の点と平面の符号付き最短距離を返す,負ならその点は平面の法線の向きに対して反対側にある
	float SignedDist(const Vector3& point) const;

	Vector3 mNormal; // 平面の法線
	float mD;		 // 平面と原点との符号付き最短距離
};

/* 以降はバウンディングボリューム */

// 球
struct Sphere
{
	Sphere(const Vector3& center, float radius);
	bool Contains(const Vector3& point) const;

	Vector3 mCenter;
	float mRadius;
};

// axis-aligned bounding box 軸平行バウンディングボックス
// すべての面の法線が基本ベクトルのどれかと平行という性質(制限ともいえる)がある
// したがって回転の際にはボックスをクルっと回せないので、代わりに寸法を変更する
struct AABB
{
	AABB(const Vector3& min, const Vector3& max);
	// Update min and max accounting for this point
	// (used when loading a model)
	void UpdateMinMax(const Vector3& point);
	// Rotated by a quaternion
	void Rotate(const Quaternion& q);
	bool Contains(const Vector3& point) const;
	float MinDistSq(const Vector3& point) const;

	Vector3 mMin;	// 最大点(最大のx,y,zを持つ点)
	Vector3 mMax;	// 最小点(最小のx,y,zを持つ点)
};

// oriented bounding box 有向バウンディングボックス
// AABBと違って,回転できる(正確には,回転しても境界が緩まない)バージョン
struct OBB
{
	Vector3 mCenter;
	Quaternion mRotation;
	Vector3 mExtents;
};

// カプセル...半径を持つ線分
struct Capsule
{
	Capsule(const Vector3& start, const Vector3& end, float radius);
	// Get point along segment where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	bool Contains(const Vector3& point) const;

	LineSegment mSegment; // 線分
	float mRadius;		  // 半径
};

// 凸ポリゴン(多角形) ただし2D
struct ConvexPolygon
{
	bool Contains(const Vector2& point) const;
	// 頂点の順序は時計回り
	std::vector<Vector2> mVertices;
};

// バウンディングボリューム間の交差判定
bool Intersect(const Sphere& a, const Sphere& b);	// 球と球
bool Intersect(const AABB& a, const AABB& b);		// AABBとAABB
bool Intersect(const Capsule& a, const Capsule& b); // カプセルとカプセル
bool Intersect(const Sphere& s, const AABB& box);	// 球とAABB
// 線分と他の境界立体
bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& t);
