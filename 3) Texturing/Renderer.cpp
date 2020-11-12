#include"Renderer.h"
Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	triangle = Mesh::GenerateTriangle();
	// 	texture = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	texture = SOIL_load_OGL_texture(TEXTUREDIR "brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!texture) {
		return;
	}
	shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");

	if (!shader->LoadSuccess()) {
		return;
	}
	biFiltering = false;
	triFiltering = false;
	anistropicFiltering = false;
	repeating = false;
	init = true;
}
Renderer ::~Renderer(void) {
	delete triangle;
	delete shader;
	glDeleteTextures(1, &texture);
}
void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0); // this last parameter
	glActiveTexture(GL_TEXTURE0); // should match this number !
	glBindTexture(GL_TEXTURE_2D, texture);
	triangle->Draw();
}
void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}
void Renderer::ToggleBilinearFiltering() {
	biFiltering = !biFiltering;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, biFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, biFiltering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::ToggleTrilinearFiltering() {
	triFiltering = !triFiltering;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, triFiltering ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::ToggleAnisotropicFiltering() {
	anistropicFiltering = !anistropicFiltering;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anistropicFiltering ? 34047.0f : 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void Renderer::ToggleRepeating() {
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeating ? GL_REPEAT : GL_CLAMP);	//x axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeating ? GL_REPEAT : GL_CLAMP);	// y axis
	glBindTexture(GL_TEXTURE_2D, 0);
}
