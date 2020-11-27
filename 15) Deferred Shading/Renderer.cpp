#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#define SHADOWSIZE 2048

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	cone = Mesh::LoadFromMeshFile("Cone.msh");
	ball = Mesh::LoadFromMeshFile("Sphere.msh");
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR "noise.png");
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	
	heightmapSize = heightMap->GetHeightmapSize();
	camera = new Camera(-45.0f, 0.0f, 0.0f, heightmapSize * Vector3(0.5f, 5.0f, 0.5f));
	lights.reserve(LIGHT_NUM);
	for (int i = 0; i < LIGHT_NUM; ++i) {
		if (i == 0) {
			lights.push_back(new DirectionalLight(Vector3(-0.2f, -1.0f, -0.3f), Vector4(0.1, 0.1, 0, 1)));
		}
		if (i == 1) {
			lights.push_back(new SpotLight(camera->GetPosition(), Vector4(1, 0, 0, 1), Vector3(0.01, -1, 0.01), heightmapSize.x * 0.5f, 15.0f));
		}
		if (i == 2) {
			lights.push_back(new PointLight(heightmapSize * Vector3(0.4f, 5.0f, 0.4f), Vector4(0, 1, 0, 1), heightmapSize.x / 2));
		}
		if (i == 3) {
			lights.push_back(new PointLight(heightmapSize * Vector3(0.6f, 5.0f, 0.6f), Vector4(0, 0, 1, 1), heightmapSize.x / 2));
		}
	}
	shadowShader = new Shader("ShadowVertex.glsl", "ShadowFragment.glsl");
	sceneShader = new Shader("BumpVertex.glsl", "BufferFragment.glsl");
	pointlightShader = new Shader("Pointlightvertex.glsl", "Pointlightfragment.glsl");
	combineShader = new Shader("CombinedVertex.glsl", "CombinedFragment.glsl");
	if (!sceneShader->LoadSuccess() || !pointlightShader->LoadSuccess() || !combineShader->LoadSuccess() || !shadowShader->LoadSuccess()) {
		return;
	}
	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	// Generate our scene depth texture ...
	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightDiffuseTex);
	GenerateScreenTexture(lightSpecularTex);
	for (int i = 0; i < LIGHT_NUM; ++i) {
		glGenTextures(1, &shadowTex[i]);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &shadowFBO[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex[i], 0);
		glDrawBuffer(GL_NONE);
	}
	glGenFramebuffers(1, &bufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glGenFramebuffers(1, &pointLightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	init = true;
	sceneTransform = Matrix4::Translation(heightmapSize * Vector3(0.5f, 2, 0.5f)) * Matrix4::Scale(Vector3(100, 100, 100));
}
Renderer ::~Renderer(void) {
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;
	delete heightMap;
	delete camera;
	delete sphere;
	delete ball;
	delete quad;
	for (auto light : lights)
		delete light;
	lights.clear();
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(LIGHT_NUM, shadowTex);
	glDeleteFramebuffers(LIGHT_NUM, shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}
void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
}
void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawShadowScene();
	FillBuffers();
	DrawPointLights();
	CombineBuffers();
}
void Renderer::DrawShadowScene() {
	BindShader(shadowShader);
	int i = 0;
	for (auto light : lights) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		if (PointLight* p = dynamic_cast<PointLight*> (light)) {
			viewMatrix = Matrix4::BuildViewMatrix(p->GetPosition(), heightmapSize * Vector3(0.5, 0, 0.5));
		}
		else if (SpotLight* s = dynamic_cast<SpotLight*> (light)) {
			viewMatrix = Matrix4::BuildViewMatrix(s->GetPosition(), s->GetPosition() + s->GetDirection());
		}
		else if (DirectionalLight* d = dynamic_cast<DirectionalLight*> (light)) {
			viewMatrix = Matrix4::BuildViewMatrix(Vector3(0, 0, 0), Vector3(0, 0, 0));
		}
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 90.0f);
		shadowMatrices[i] = projMatrix * viewMatrix; // used later
		modelMatrix.ToIdentity();
		UpdateShaderMatrices();
		heightMap->Draw();

		modelMatrix = sceneTransform;
		UpdateShaderMatrices();
		ball->Draw();
		++i;
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::FillBuffers() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	BindShader(sceneShader);
	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(sceneShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);
	
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();

	modelMatrix = sceneTransform;
	UpdateShaderMatrices();
	ball->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawPointLights() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	BindShader(pointlightShader);
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
	
	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	
	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);
	
	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	
	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	for (int i = 0; i < LIGHT_NUM; ++i) {
		SetShaderLight(lights.at(i));
		glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "shadowMatrix"), 1, false, shadowMatrices[i].values);
	
		glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "shadowTex"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
		if (dynamic_cast<SpotLight*> (lights.at(i))) {
			cone->Draw();
		}
		else {
			sphere->Draw();
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::CombineBuffers() {
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);
	
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
	
	quad->Draw();
}