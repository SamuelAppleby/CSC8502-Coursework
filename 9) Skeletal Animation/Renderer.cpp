#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	cameras.push_back(new Camera(-3.0f, 0.0f, 0.0f, Vector3(0, 1.4f, 4.0f)));
	cameras.push_back(new Camera(-3.0f, 90.0f, 0.0f, Vector3(4, 1.4f, 0.0f)));

	shader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	
	if (!shader->LoadSuccess()) {
		return;
	}
	mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	anim = new MeshAnimation("Role_T.anm");
	material = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry * matEntry = material->GetMaterialForLayer(i);
		const string * filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		matTextures.emplace_back(texID);
	}
	currentFrame = 0;
	frameTime = 0.0f;
	init = true;
	currentCamera = 0;
	onRails = false;
	splitScreen = false;
}
Renderer ::~Renderer(void) {
	for (auto camera : cameras)
		delete camera;
	cameras.clear();
	delete mesh;
	delete anim;
	delete material;
	delete shader;
}
void Renderer::UpdateScene(float dt) {
	if (!onRails) {
		for (auto camera : cameras) {
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
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			splitScreen = !splitScreen;
		}
	}
	else {
		for (auto& camera : cameras) {
			camera->MoveRight(dt);
			camera->LookLeft(dt * 15);
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R)) {
		while (!cameras.empty()) {
			delete cameras.back();
			cameras.pop_back();
		}
		cameras.push_back(new Camera(-3.0f, 0.0f, 0.0f, Vector3(0, 1.4f, 4.0f)));
		cameras.push_back(new Camera(-3.0f, 90.0f, 0.0f, Vector3(4, 1.4f, 0.0f)));
		onRails = !onRails;
	}
	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}
}
void Renderer::RenderScene() {
	if (splitScreen) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width / 2, height);
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / 2 / (float)height, 45.0f);
		viewMatrix = cameras.at(0)->BuildViewMatrix();
		DrawScene();

		glViewport(width / 2, 0, width / 2, height);
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / 2 / (float)height, 45.0f);
		viewMatrix = cameras.at(1)->BuildViewMatrix();
		DrawScene();
	}
	else {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, width, height);
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		viewMatrix = cameras.at(0)->BuildViewMatrix();
		DrawScene();
	}
}
void Renderer::DrawScene() {
	BindShader(shader);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	vector <Matrix4> frameMatrices;

	const Matrix4* invBindPose = mesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < mesh->GetJointCount(); ++i) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	for (int i = 0; i < mesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matTextures[i]);
		mesh->DrawSubMesh(i);
	}
}
void Renderer::SwitchCamera(int newCam) {
	cameras.at(currentCamera)->setEnabled(false);
	cameras.at(newCam)->setEnabled(true);
	currentCamera = newCam;
}