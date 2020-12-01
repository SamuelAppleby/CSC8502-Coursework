# pragma once
#include"../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	void UpdateTextureMatrix(float rotation);

protected:
	Shader * shader;
	Mesh * triangle;
	vector<GLuint> textures;

	bool biFiltering;
	bool triFiltering;
	bool anisotropicFiltering;
	bool repeating;
};
