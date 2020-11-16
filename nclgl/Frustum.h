#pragma once
#include "Plane.h"
#include "BoundingVolume.h"
#include "BoundingSphere.h"
class SceneNode; // These let the compiler know that these keywords
class Matrix4; // are classes , without including the whole header

class Frustum {
public:
	Frustum(void) {};
	~Frustum(void) {};
	bool InsideFrustum(SceneNode& n);
	void FromMatrix(const Matrix4 & mvp);
protected:
	Plane planes[6];
};
