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

// ����
struct LineSegment
{
	LineSegment(const Vector3& start, const Vector3& end);
	// ������̓_��Ԃ� (0 <= t <= 1)
	Vector3 PointOnSegment(float t) const;
	// �_�Ɛ����̍ŒZ�����̂Q���Ԃ�
	float MinDistSq(const Vector3& point) const;
	// Get MinDistSq between two line segments
	static float MinDistSq(const LineSegment& s1, const LineSegment& s2);

	Vector3 mStart;	// �n�_
	Vector3 mEnd;	// �I�_
};

// ����
struct Plane
{
	Plane(const Vector3& normal, float d);
	// 3�̓_���畽�ʂ��\�z����
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);
	// �C�ӂ̓_�ƕ��ʂ̕����t���ŒZ������Ԃ�,���Ȃ炻�̓_�͕��ʂ̖@���̌����ɑ΂��Ĕ��Α��ɂ���
	float SignedDist(const Vector3& point) const;

	Vector3 mNormal; // ���ʂ̖@��
	float mD;		 // ���ʂƌ��_�Ƃ̕����t���ŒZ����
};

/* �ȍ~�̓o�E���f�B���O�{�����[�� */

// ��
struct Sphere
{
	Sphere(const Vector3& center, float radius);
	bool Contains(const Vector3& point) const;

	Vector3 mCenter;
	float mRadius;
};

// axis-aligned bounding box �����s�o�E���f�B���O�{�b�N�X
// ���ׂĂ̖ʂ̖@������{�x�N�g���̂ǂꂩ�ƕ��s�Ƃ�������(�����Ƃ�������)������
// ���������ĉ�]�̍ۂɂ̓{�b�N�X���N�����Ɖ񂹂Ȃ��̂ŁA����ɐ��@��ύX����
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

	Vector3 mMin;	// �ő�_(�ő��x,y,z�����_)
	Vector3 mMax;	// �ŏ��_(�ŏ���x,y,z�����_)
};

// oriented bounding box �L���o�E���f�B���O�{�b�N�X
// AABB�ƈ����,��]�ł���(���m�ɂ�,��]���Ă����E���ɂ܂Ȃ�)�o�[�W����
struct OBB
{
	Vector3 mCenter;
	Quaternion mRotation;
	Vector3 mExtents;
};

// �J�v�Z��...���a��������
struct Capsule
{
	Capsule(const Vector3& start, const Vector3& end, float radius);
	// Get point along segment where 0 <= t <= 1
	Vector3 PointOnSegment(float t) const;
	bool Contains(const Vector3& point) const;

	LineSegment mSegment; // ����
	float mRadius;		  // ���a
};

// �ʃ|���S��(���p�`) ������2D
struct ConvexPolygon
{
	bool Contains(const Vector2& point) const;
	// ���_�̏����͎��v���
	std::vector<Vector2> mVertices;
};

// �o�E���f�B���O�{�����[���Ԃ̌�������
bool Intersect(const Sphere& a, const Sphere& b);	// ���Ƌ�
bool Intersect(const AABB& a, const AABB& b);		// AABB��AABB
bool Intersect(const Capsule& a, const Capsule& b); // �J�v�Z���ƃJ�v�Z��
bool Intersect(const Sphere& s, const AABB& box);	// ����AABB
// �����Ƒ��̋��E����
bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT,
	Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1,
	const Sphere& Q0, const Sphere& Q1, float& t);
