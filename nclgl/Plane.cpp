#include "Plane.h"
Plane::Plane(const Vector3 & normal, float distance, bool normalise) {
	if (normalise) {
		float length = sqrt(Vector3::Dot(normal, normal));
		this->normal = normal / length;
		this->distance = distance / length;
	}
	else {
		this-> normal = normal;
		this->distance = distance;
	}
}
bool Plane::SphereInPlane(const Vector3& position, BoundingSphere* sphere) const {
	return Vector3::Dot(position, normal) + distance <= -sphere->getRadius() ? false :  true;
}
bool Plane::AABBInPlane(const Vector3& position, BoundingAABB* AABB) const {
	Vector3 extents = (AABB->getMax() - AABB->getMin()) / 2.0f;
	Vector3 centre = AABB->getMin() + extents;
	float fRadius = abs(extents.x * normal.x) + abs(extents.y * normal.y) + abs(extents.z * normal.z);
	return SphereInPlane(position, new BoundingSphere(fRadius));
}

