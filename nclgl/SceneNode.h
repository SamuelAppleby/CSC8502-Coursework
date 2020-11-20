#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "BoundingVolume.h"
#include "BoundingSphere.h"
#include "BoundingAABB.h"
class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);
	void SetTransform(const Matrix4 & matrix) { transform = matrix; }
	const Matrix4 & GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }
	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }
	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }
	Mesh * GetMesh() const { return mesh; }
	void SetMesh(Mesh * m) { mesh = m; }
	std::vector<SceneNode*>& getChildren() {
		return children;
	}
	void AddChild(SceneNode* s);
	void RemoveChild(SceneNode* s);
	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }
	BoundingVolume* getBoundingVolume() const { return boundingVolume; }
	void setBoundingVolume(BoundingVolume* boundingVolume) { this->boundingVolume = boundingVolume; }
	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }
	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}
	bool considerChilren() { return considerChildren; }
	virtual void Draw(const OGLRenderer& r);
	virtual void Update(float dt);

protected:
	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector <SceneNode*> children;
	float distanceFromCamera;
	BoundingVolume* boundingVolume;
	GLuint texture;
	bool considerChildren;
};
