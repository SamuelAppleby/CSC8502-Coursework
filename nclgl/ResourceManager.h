#pragma once
#include "OGLRenderer.h"
#include "MeshAnimation.h"
#include "MeshMaterial.h"
#include "Camera.h"
class ResourceManager {
public:
	~ResourceManager() {
		for (auto& shader : sceneShaders)
			delete shader;
		for (auto& mesh : sceneMeshes)
			delete mesh;
		for (auto& material : sceneMaterials)
			delete material;
		for (auto& animation : sceneAnimations)
			delete animation;
		for (auto& light : lights)
			delete light;
		for (auto& camera : cameras)
			delete camera;
	}
	vector<Shader*> sceneShaders;
	vector<GLuint> sceneTextures;
	vector<Mesh*> sceneMeshes;
	vector<Matrix4> sceneTransforms;

	vector<MeshMaterial*> sceneMaterials;
	vector<MeshAnimation*> sceneAnimations;
	vector <vector<GLuint>> matTextures;

	vector<Light*> lights;
	vector<Camera*> cameras;	
};

