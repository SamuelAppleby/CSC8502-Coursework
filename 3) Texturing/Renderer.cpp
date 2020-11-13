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
	anisotropicFiltering = false;
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
	if (triFiltering)
		ToggleTrilinearFiltering();
	biFiltering = !biFiltering;
	SetBilinearFiltering(texture, biFiltering);
	biFiltering ? std::cout << "BiLinear Filtering ON!" << std::endl : std::cout << "BiLinear Filtering OFF!" << std::endl;
}
void Renderer::ToggleTrilinearFiltering() {
	if (biFiltering)
		ToggleBilinearFiltering();
	triFiltering = !triFiltering;
	SetTrilinearFiltering(texture, triFiltering);
	triFiltering ? std::cout << "Trilinear Filtering ON!" << std::endl : std::cout << "Trilinear Filtering OFF!" << std::endl;
}
void Renderer::ToggleAnisotropicFiltering() {
	anisotropicFiltering = !anisotropicFiltering;
	GLfloat value, max_anisotropy = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);
	value = (value > max_anisotropy) ? max_anisotropy : value;
	SetAnisotropicFiltering(texture, anisotropicFiltering, value);
	anisotropicFiltering ? std::cout << "AnistropicFiltering Filtering ON: " << value << "x"
		<< std::endl : std::cout << "AnistropicFiltering Filtering OFF!" << std::endl;
}
void Renderer::ToggleRepeating() {
	repeating = !repeating;
	SetTextureRepeating(texture, repeating);
}
