/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069
 *				Light base class					*/
#pragma once
#include "Vector4.h"
#include "Vector3.h"
class Light {
public:
	Light() {} // Default constructor , we ’ll be needing this later !
	Light(const Vector4& colour) {
		this->colour = colour;
	}
	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }
	virtual void print() {
		std::cout << "Parent Light" << std::endl;
	};
protected:
	Vector4 colour;
};

