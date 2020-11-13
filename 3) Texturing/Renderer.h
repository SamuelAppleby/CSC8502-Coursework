# pragma once
#include"../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	void UpdateTextureMatrix(float rotation);
	void ToggleBilinearFiltering();
	void ToggleTrilinearFiltering();
	void ToggleAnisotropicFiltering();
	void ToggleRepeating();

protected:
	Shader * shader;
	Mesh * triangle;
	GLuint texture;
	GLuint texture1;

	bool biFiltering;
	bool triFiltering;
	bool anisotropicFiltering;
	bool repeating;
};
