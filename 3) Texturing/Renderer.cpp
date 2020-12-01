#include"Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	// 	texture = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);	// No MipMap
	textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	if (!shader->LoadSuccess()) {
		return;
	}
	biFiltering = false;
	triFiltering = false;
	anisotropicFiltering = false;
	repeating = false;
	init = true;
}
Renderer ::~Renderer(void) {
	delete triangle;
	delete shader;
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0); // this last parameter
	glActiveTexture(GL_TEXTURE0); // should match this number !
	glBindTexture(GL_TEXTURE_2D, textures.at(0));
	triangle->Draw();
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
		ToggleBilinearFiltering(textures);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		ToggleTrilinearFiltering(textures);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
		ToggleAnisotropicFiltering(textures);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
		SetTextureRepeating(textures.at(0), true);
	}
}
void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}
