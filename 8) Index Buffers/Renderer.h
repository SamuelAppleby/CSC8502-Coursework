#pragma once
#include "../nclgl/OGLRenderer.h"
class Mesh;
class Camera;
class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	void DrawScene();
	void SwitchCamera(int newCam);
protected:
	Mesh * heightMap;
	Mesh* cube;
	Shader * shader;
	vector<Camera*> cameras;
	int currentCamera;
	GLuint terrainTex;
};
