#pragma once
#include "Vector3.h"
#include "mesh.h"
class Particle
{
public:
	Particle(Mesh* mesh, Vector3 position) {
		this->mesh = mesh;
		this->position = position;
	}
	Mesh* GetMesh() {
		return mesh;
	}
	void SetMesh(Mesh* mesh) {
		this->mesh = mesh;
	}
	Vector3 GetPosition() {
		return position;
	}
	void SetPosition(Vector3 position) {
		this->position = position;
	}
private:
	Mesh* mesh;
	Vector3 position;
};

