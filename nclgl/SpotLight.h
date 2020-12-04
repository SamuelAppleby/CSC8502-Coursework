/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069
 *				Spotlight class					*/
#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "PointLight.h"
class SpotLight : public Light {
public:
	SpotLight() {} 
	SpotLight(const Vector3& position, const Vector4& colour, const Vector3& direction, float radius, float maxAngle) : Light(colour) {
		this->position = position;
		this->direction = direction;
		this->radius = radius;
		this->maxAngle = maxAngle;
	}
	~SpotLight(void) {};
	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	Vector3 GetDirection() const { return direction; }
	void SetDirection(Vector3 val) { direction = val; }
	float GetRadius() const { return radius; }
	void SetDirection(float val) { radius = val; }
	float GetAngle() const { return maxAngle; }
	void SetAngle(float val) { maxAngle = val; }
protected:
	Vector3 position;
	Vector3 direction;
	float radius;
	float maxAngle;
};
