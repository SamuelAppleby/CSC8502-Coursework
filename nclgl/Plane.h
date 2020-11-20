#pragma once
#include "Vector3.h"
#include "BoundingSphere.h"
#include "BoundingAABB.h"
class Plane {
public:
	Plane(void) {};
	Plane(const Vector3 & normal, float distance, bool normalise = false);
	~Plane(void) {};
	void SetNormal(const Vector3 & normal) { this->normal = normal; }
	Vector3 GetNormal() const { return normal; }
	void SetDistance(float dist) { distance = dist; }
	float GetDistance() const { return distance; }
	bool SphereInPlane(const Vector3 & position, BoundingSphere* sphere) const;
	bool AABBInPlane(const Vector3& position, BoundingAABB* AABB) const;
protected:
	Vector3 normal;
	float distance;
};
