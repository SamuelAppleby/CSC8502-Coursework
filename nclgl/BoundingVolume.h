#pragma once
#include <iostream>
class BoundingVolume {
public:
	virtual void print() {
		std::cout << "Parent Volume" << std::endl;
	};
};

