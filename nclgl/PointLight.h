/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069
 *				Point light class				*/
#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Light.h"
class PointLight : public Light {
public:
	PointLight() {} 
	PointLight(const Vector3& position, const Vector4& colour, float radius) : Light(colour) {
		this->position = position;
		this->radius = radius;
	}
	~PointLight(void) {};
	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }
	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }
protected:
	Vector3 position;
	float radius;
};
