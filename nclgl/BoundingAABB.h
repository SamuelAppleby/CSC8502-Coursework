#pragma once
#include"BoundingVolume.h"
class BoundingAABB : public BoundingVolume {
private:
	float halfLength;
	float halfWidth;
	float halfDepth;
public:
	BoundingAABB(float halfLength, float halfWidth, float halfDepth) {
		this->halfLength = halfLength;
		this->halfWidth = halfWidth;
		this->halfDepth = halfDepth;
	}
	float getHalfLength() { return halfLength; }
	void setHalfLength(float halfLength) { this->halfLength = halfLength; }
	float getHalfWidth() { return halfWidth; }
	void setHalfWidth(float halfWidth) { this->halfWidth = halfWidth; }
	float getHalfDepth() { return halfDepth; }
	void setHalfDepth(float halfDepth) { this->halfDepth = halfDepth; }
	void print() { std::cout << "Bounding AABB: " << halfLength << ", " << halfWidth << ", " << halfDepth << std::endl; }
};

