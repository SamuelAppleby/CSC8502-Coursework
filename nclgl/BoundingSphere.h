#pragma once
#include"BoundingVolume.h"
class BoundingSphere : public BoundingVolume {
private:
	float radius;
public:
	BoundingSphere(float radius) {
		this->radius = radius;
	}
	float getRadius() { return radius; }
	void setRadius(float radius) { this->radius = radius; }
	void print() { std::cout << "Bounding Sphere: " << radius << std::endl; }
};

