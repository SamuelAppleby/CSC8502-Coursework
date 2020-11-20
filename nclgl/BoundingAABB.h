#pragma once
#include"BoundingVolume.h"
class BoundingAABB : public BoundingVolume {
private:
	Vector3 min;
	Vector3 max;
public:
	BoundingAABB(Vector3 min, Vector3 max) {
		this->min = min;
		this->max = max;
	}
	Vector3 getMin() { return min; }
	void setMin(Vector3 min) { this->min = min; }
	Vector3 getMax() { return max; }
	void setMax(Vector3 max) { this->max = max; }
	void print() { std::cout << "Bounding AABB: " << min << ", " << max << std::endl; }
};

