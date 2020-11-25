# pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	void SwitchCamera(int newCam);
protected:
	void PresentScene(bool clear);
	void DrawPostProcess();
	void DrawScene();
	
	Shader * sceneShader;
	Shader * processShader;
	
	vector<Camera*> cameras;

	Mesh * quad;
	HeightMap * heightMap;
	GLuint heightTexture;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	int currentCamera;
};
