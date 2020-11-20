#include "SceneNode.h"
SceneNode::SceneNode(Mesh* mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	modelScale = Vector3(1, 1, 1);
	distanceFromCamera = 0.0f;
	boundingVolume = new BoundingVolume;
	texture = 0;
	considerChildren = true;
}
SceneNode ::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
	delete boundingVolume;
}
void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}
void SceneNode::RemoveChild(SceneNode* s) {
	children.erase(std::remove(children.begin(), children.end(), s), children.end());
}
void SceneNode::Draw(const OGLRenderer& r) {
	if (mesh)
		mesh->Draw();
}
void SceneNode::Update(float dt) {
	if (parent) { // This node has a parent ...
		worldTransform = parent->worldTransform * transform;
	}
	else { // Root node , world transform is local transform !
		worldTransform = transform;
	}
	for (auto i : children) {
		i->Update(dt);
	}
}