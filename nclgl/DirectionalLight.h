#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Light.h"
class DirectionalLight : public Light {
public:
	DirectionalLight() {} 
	DirectionalLight(const Vector3& direction, const Vector4& colour) : Light(colour) {
		this->direction = direction;
	}
	~DirectionalLight(void) {};
	Vector3 GetDirection() const { return direction; }
	void SetDirection(const Vector3& val) { direction = val; }
protected:
	Vector3 direction;
};
