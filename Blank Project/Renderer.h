#pragma once
#include "../nclgl/OGLRenderer.h"
#include <nclgl\HeightMap.h>
#include <nclgl\Particle.h>
#include <nclgl\ResourceManager.h>
#include <nclgl\MeshAnimation.h>
#include <nclgl\MeshMaterial.h>
const int LIGHT_NUM = 13;
const int POST_PASSES = 6;

class Camera;
class Mesh;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void UpdateScene(float dt) override;
	void RenderScene() override;
protected:
	void TraverseScene();
	void AnimateObjects();
	void DayNightCycle();
	void DrawSkybox();
	void DrawShadowScene();
	void DrawMainScene(int camera);
	void DrawRain();
	void DrawPostProcess();
	void PresentScene();
	bool SetTexture(GLuint texID, GLuint unit, const std::string& uniformName, Shader* s);
	GLuint shadowFBO[LIGHT_NUM];
	GLuint shadowTex[LIGHT_NUM];
	Matrix4 shadowMatrices[LIGHT_NUM];

	ResourceManager* resources;
	float sceneTime;
	float brightness;
	bool daySwap;
	float frameTime;
	float dayTime;
	float restTime;
	float waterCycle;
	bool dayCycle;
	bool resetCam;
	Vector3 heightmapSize;
	HeightMap* heightMap;
	GLuint cubeMap;

	int currentFrame;
	float animTime;

	unsigned int nr_particles = 3000;
	vector<Particle> particles;
	bool rainInit;

	bool onRails;
	bool turning;
	bool forward;
	bool postProcess;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	bool flashlight;
};