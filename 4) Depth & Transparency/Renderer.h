# pragma once
#include"../nclgl/OGLRenderer.h"
class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	~Renderer(void);
	void RenderScene() override;
	void MoveObject(float by);
	void ToggleObject();
	void ToggleDepth();
	void ToggleDepthMask();
	void ToggleDepthMode();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	
protected:
	GLuint textures[2];
	Mesh* meshes[2];
	Shader* shader;
	Vector3 positions[2];
	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;
	int depthMode;
	bool depthMask;
};
