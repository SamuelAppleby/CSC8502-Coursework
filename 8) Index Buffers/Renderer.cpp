#include "Renderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR "noise.png");
	cameras.push_back(new Camera(-40, 270, 0, Vector3(-2100, 3300, 2000)));
	cameras.push_back(new Camera(-40, 270, -45, Vector3(1000, 3300, 2000)));
	shader = new Shader("ShadowVertex.glsl", "ShadowFragment.glsl");
	if (!shader->LoadSuccess()) {
		return;
	}
	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	if (!terrainTex) {
		return;
	}
	SetTextureRepeating(terrainTex, true);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
	currentCamera = 0;
}
Renderer ::~Renderer(void) {
	for (auto camera : cameras)
		delete camera;
	cameras.clear();
	delete heightMap;
	delete shader;
}
void Renderer::UpdateScene(float dt) {
	for (auto& camera : cameras) {
		if (camera->isEnabled()) {
			camera->UpdateCamera(dt);
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
		SwitchCamera(0);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		SwitchCamera(1);
	}
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width / 2, height);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / 2 / (float)height, 45.0f);
	viewMatrix = cameras.at(0)->BuildViewMatrix();
	DrawScene();

	glViewport(width / 2, 0, width / 2, height);
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / 2 / (float)height, 45.0f);
	viewMatrix = cameras.at(1)->BuildViewMatrix();
	DrawScene();
}
void Renderer::DrawScene() {
	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	heightMap->Draw();
}

void Renderer::SwitchCamera(int newCam) {
	cameras.at(currentCamera)->setEnabled(false);
	cameras.at(newCam)->setEnabled(true);
	currentCamera = newCam;
}
