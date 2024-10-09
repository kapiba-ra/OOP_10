// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
// MinDistSq between two line segments:
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// ----------------------------------------------------------------

#include "Collision.h"
#include <algorithm>
#include <array>

LineSegment::LineSegment(const Vector3& start, const Vector3& end)
	: mStart(start)
	, mEnd(end)
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
	return mStart + (mEnd - mStart) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
	// Construct vectors
	Vector3 ab = mEnd - mStart;
	Vector3 ba = -1.0f * ab;
	Vector3 ac = point - mStart;
	Vector3 bc = point - mEnd;

	// Case 1: C projects prior to A
	// abとacがなす角が鈍角であるケース
	if (Vector3::Dot(ab, ac) < 0.0f)
	{
		return ac.LengthSq();
	}
	// Case 2: C projects after B
	// baとbcのなす角が鈍角であるケース
	else if (Vector3::Dot(ba, bc) < 0.0f)
	{
		return bc.LengthSq();
	}
	// Case 3: C projects onto line
	// 線分ab上に点cが投影されるようなケース,abとac,baとbcがなす角が共に鋭角
	else
	{
		// Compute p(ap) 以下の手順でapベクトルが求まる
		// Dot(ac, ab) = |ac|*|ab|*cos = |ac|*|ab|*|ap|/|ac|. scalar = |ap|/|ab|
		// p(ap) = (|ap|/|ab|) * ab = |ap| * ab/|ab|
		float scalar = Vector3::Dot(ac, ab) / Vector3::Dot(ab, ab);
		Vector3 p = scalar * ab;
		// Compute length squared of ac - p (ac-pはpcの長さ,returnするのはその2乗)
		return (ac - p).LengthSq();
	}
}

float LineSegment::MinDistSq(const LineSegment& s1, const LineSegment& s2)
{
	// 線分と線分の最短距離の計算,エッジケースが複数ある
	Vector3   u = s1.mEnd - s1.mStart;
	Vector3   v = s2.mEnd - s2.mStart;
	Vector3   w = s1.mStart - s2.mStart;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a * c - b * b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (Math::NearZero(D)) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
	tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.LengthSq();   // return the closest distance squared
}

Plane::Plane(const Vector3& normal, float d)
	:mNormal(normal)
	, mD(d)
{
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	// abとac
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	// クロス積して正規化し,大きさ1の法線ベクトルを得る
	mNormal = Vector3::Cross(ab, ac);
	mNormal.Normalize();
	// d = -P dot n (平面の方程式 : P dot n + d = 0 より)
	mD = -Vector3::Dot(a, mNormal);
}

float Plane::SignedDist(const Vector3& point) const
{
	return Vector3::Dot(point, mNormal) - mD;
}

Sphere::Sphere(const Vector3& center, float radius)
	:mCenter(center)
	, mRadius(radius)
{
}

bool Sphere::Contains(const Vector3& point) const
{
	// 中心と点との距離の2乗を求める(計算がラク)
	float distSq = (mCenter - point).LengthSq();
	// 球内部に点が含まれるならtrueを返す
	return distSq <= (mRadius * mRadius);
}

AABB::AABB(const Vector3& min, const Vector3& max)
	: mMin(min)
	, mMax(max)
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
	// Update each component separately
	// 各成分を独立して更新する
	mMin.x = Math::Min(mMin.x, point.x);
	mMin.y = Math::Min(mMin.y, point.y);
	mMin.z = Math::Min(mMin.z, point.z);

	mMax.x = Math::Max(mMax.x, point.x);
	mMax.y = Math::Max(mMax.y, point.y);
	mMax.z = Math::Max(mMax.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	// ボックスの角の計8点を配列に入れる
	std::array<Vector3, 8> points;
	// 最小点
	points[0] = mMin;
	// 2個のminと1個のmaxの順列組み合わせ
	points[1] = Vector3(mMax.x, mMin.y, mMin.z);
	points[2] = Vector3(mMin.x, mMax.y, mMin.z);
	points[3] = Vector3(mMin.x, mMin.y, mMax.z);
	// 2個のmaxと1個のminの順列組み合わせ
	points[4] = Vector3(mMin.x, mMax.y, mMax.z);
	points[5] = Vector3(mMax.x, mMin.y, mMax.z);
	points[6] = Vector3(mMax.x, mMax.y, mMin.z);
	// 最大点
	points[7] = Vector3(mMax);

	// 最初の点を回転する
	Vector3 p = Vector3::Transform(points[0], q);
	// 回転した第１の点で min,maxをリセット
	mMin = p;
	mMax = p;
	// 残りの回転後の点によってmin,maxを更新
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3::Transform(points[i], q);
		UpdateMinMax(p);
	}
}

bool AABB::Contains(const Vector3& point) const
{
	bool outside = point.x < mMin.x ||
		point.y < mMin.y ||
		point.z < mMin.z ||
		point.x > mMax.x ||
		point.y > mMax.y ||
		point.z > mMax.z;
	// どれも真でないならばtrueを返す
	return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
	// 各軸についてpointとの距離dを求める(min<point<maxの場合d=0としている)
	float dx = Math::Max(mMin.x - point.x, 0.0f);
	dx = Math::Max(dx, point.x - mMax.x);
	float dy = Math::Max(mMin.y - point.y, 0.0f);
	dy = Math::Max(dy, point.y - mMax.y);
	float dz = Math::Max(mMin.z - point.z, 0.0f);
	dz = Math::Max(dy, point.z - mMax.z);
	// 距離の2乗を返す
	return dx * dx + dy * dy + dz * dz;
}

Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)
	:mSegment(start, end)
	, mRadius(radius)
{
}

Vector3 Capsule::PointOnSegment(float t) const
{
	return mSegment.PointOnSegment(t);
}

bool Capsule::Contains(const Vector3& point) const
{
	// 点と線分との最短距離の2乗を求める
	float distSq = mSegment.MinDistSq(point);
	return distSq <= (mRadius * mRadius);
}

bool ConvexPolygon::Contains(const Vector2& point) const
{
	float sum = 0.0f;
	Vector2 a, b;
	for (size_t i = 0; i < mVertices.size() - 1; i++)
	{
		// 点から第1頂点へのベクトル
		a = mVertices[i] - point;
		a.Normalize();
		// 点から第2頂点へのベクトル
		b = mVertices[i + 1] - point;
		b.Normalize();
		// 2つのベクトルがなす角度を合計に加算
		sum += Math::Acos(Vector2::Dot(a, b));
	}
	// Have to add angle for last vertex and first vertex
	a = mVertices.back() - point;
	a.Normalize();
	b = mVertices.front() - point;
	b.Normalize();
	sum += Math::Acos(Vector2::Dot(a, b));
	// Return true if approximately 2pi
	return Math::NearZero(sum - Math::TwoPi);
}

bool Intersect(const Sphere& a, const Sphere& b)
{
	float distSq = (a.mCenter - b.mCenter).LengthSq();
	float sumRadii = a.mRadius + b.mRadius;
	return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const AABB& a, const AABB& b)
{
	bool no = a.mMax.x < b.mMin.x ||
		a.mMax.y < b.mMin.y ||
		a.mMax.z < b.mMin.z ||
		b.mMax.x < a.mMin.x ||
		b.mMax.y < a.mMin.y ||
		b.mMax.z < a.mMin.z;
	// If none of these are true, they must intersect
	return !no;
}

bool Intersect(const Capsule& a, const Capsule& b)
{
	float distSq = LineSegment::MinDistSq(a.mSegment, b.mSegment);
	float sumRadii = a.mRadius + b.mRadius;
	return distSq <= (sumRadii * sumRadii);
}

bool Intersect(const Sphere& s, const AABB& box)
{
	float distSq = box.MinDistSq(s.mCenter);
	return distSq <= (s.mRadius * s.mRadius);
}

bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
	// 方程式のX,Y,a,b,cを計算
	Vector3 X = l.mStart - s.mCenter;
	Vector3 Y = l.mEnd - l.mStart;
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float c = Vector3::Dot(X, X) - s.mRadius * s.mRadius;
	// 判別式を計算
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = Math::Sqrt(disc);
		// tの解を求める
		float tMin = (-b - disc) / (2.0f * a);
		float tMax = (-b + disc) / (2.0f * a);
		// tが線分の領域(0 <= t <= 1)にあるかどうか調べる
		if (tMin >= 0.0f && tMin <= 1.0f)
		{
			outT = tMin;
			return true;
		}
		else if (tMax >= 0.0f && tMax <= 1.0f)
		{
			outT = tMax;
			return true;
		}
		else // 球の内部に線分がある場合もfalseを返している事に注意
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
	// 最初にtの解が存在するのかをテストする
	// denom : 分母
	float denom = Vector3::Dot(l.mEnd - l.mStart, p.mNormal);
	if (Math::NearZero(denom))
	{
		// 交差の可能性があるのは、唯一start/endが平面上の点であるとき
		// すなわち、P dot N == d の場合のみ
		if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal) - p.mD))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// numer : 分子 
		float numer = -Vector3::Dot(l.mStart, p.mNormal) -p.mD;
		outT = numer / denom;	// outTは参照であることに注意
		// tが線分の境界内にあるか
		if (outT >= 0.0f && outT <= 1.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool TestSidePlane(float start, float end, float negd, const Vector3& norm,
	std::vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (Math::NearZero(denom))
	{
		return false;
	}
	else
	{
		float numer = -start + negd;
		float t = numer / denom;
		// Test that t is within bounds
		if (t >= 0.0f && t <= 1.0f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm)
{
	// Vector to save all possible t values, and normals for those sides
	std::vector<std::pair<float, Vector3>> tValues;
	// Test the x planes
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX,
		tValues);
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX,
		tValues);
	// Test the y planes
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY,
		tValues);
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY,
		tValues);
	// Test the z planes
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ,
		tValues);
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ,
		tValues);

	// Sort the t values in ascending order
	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b) {
			return a.first < b.first;
		});
	// Test if the box contains any of these points of intersection
	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contains(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}

	//None of the intersections are within bounds of box
	return false;
}

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& outT)
{
	// Compute X, Y, a, b, and c
	Vector3 X = P0.mCenter - Q0.mCenter;
	Vector3 Y = P1.mCenter - P0.mCenter -
		(Q1.mCenter - Q0.mCenter);
	float a = Vector3::Dot(Y, Y);
	float b = 2.0f * Vector3::Dot(X, Y);
	float sumRadii = P0.mRadius + Q0.mRadius;
	float c = Vector3::Dot(X, X) - sumRadii * sumRadii;
	// 判別式を解いている
	float disc = b * b - 4.0f * a * c;
	if (disc < 0.0f)
	{
		return false;
	}
	else
	{
		disc = Math::Sqrt(disc);
		// 小さい方の解だけが必要(-b+discが分子になる方は無視)
		outT = (-b - disc) / (2.0f * a);
		if (outT >= 0.0f && outT <= 0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
