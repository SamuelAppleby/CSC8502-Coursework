/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069                   
 *			Implementing the renderer			*/
#include "Renderer.h"
#define SHADOWSIZE 4056		// Reduce shadow acne 
#include <algorithm>
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	resources = new ResourceManager();
	heightMap = new HeightMap(TEXTUREDIR "rivermap.png");
	heightmapSize = heightMap->GetHeightmapSize();

	/* Cameras */
	resources->cameras.push_back(new Camera(-45.0f, 45, 0.0f, heightmapSize * Vector3(1, 30.0f, 1)));		// Main camera
	resources->cameras.push_back(new Camera(-90, 0, 0.0f, heightmapSize * Vector3(0.5, 50.0f, 0.5)));	// Mini-map

	/* Shaders */
	resources->sceneShaders.push_back(new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("ShadowVertex.glsl", "ShadowFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("ShadowSceneVertex.glsl", "LightingFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("RainVertex.glsl", "RainFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("TexturedVertex.glsl", "ProcessBlurFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("TexturedVertex.glsl", "ProcessBloomFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("TexturedVertex.glsl", "ProcessGradingFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("TexturedVertex.glsl", "ProcessWaveFragment.glsl"));
	resources->sceneShaders.push_back(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"));
	for (auto& shader : resources->sceneShaders) {
		if (!shader->LoadSuccess())
			return;
	}
		
	/* Meshes */
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Sphere.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Cylinder.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Cone.msh"));
	resources->sceneMeshes.push_back(Mesh::GenerateQuad());
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Role_T.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("b1.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("b2.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("portrait_board.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("cube.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Road.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("Lampost.msh"));
	resources->sceneMeshes.push_back(Mesh::LoadFromMeshFile("car.msh"));
	for (auto& mesh : resources->sceneMeshes) {
		if (!mesh)
			return;
	}

	/* Mesh Materials */
	resources->sceneMaterials.push_back(new MeshMaterial("Role_T.mat"));
	for (auto& material : resources->sceneMaterials) {
		if (!material)
			return;
	}

	/* Sub-Mesh materials */
	vector<GLuint> newMaterials;
	resources->matTextures.push_back(newMaterials);
	for (int i = 0; i < resources->sceneMeshes.at(4)->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = resources->sceneMaterials.at(0)->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		resources->matTextures.at(0).emplace_back(texID);
	}

	/* Animations */
	resources->sceneAnimations.push_back(new MeshAnimation("Role_T.anm"));
	for (auto& anim : resources->sceneAnimations) {
		if (!anim)
			return;
	}

	/* Scene Transforms */
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.1, 0, 0.1) + Vector3(0, -2000, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(-0.4, 0, -0.4) + Vector3(0, -2000, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(-0.3f, 0, 0.2) + Vector3(0, -1600, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.3, 0, -0.6) + Vector3(0, -1600, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.1, 1, 0.3) + Vector3(0, -500, 0)) * Matrix4::Rotation(45, Vector3(0, 1, 0)) 
		* Matrix4::Scale(Vector3(150, 150, 150)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * 0.5f) * Matrix4::Scale(heightmapSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.62, 0.8, 0.60)) * Matrix4::Scale(Vector3(100, 100, 100)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-600, -25780, -65100)) * Matrix4::Scale(Vector3(80, 150, 285)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-32800, -24000, -8500)) * Matrix4::Scale(Vector3(300, 300, 300)) * 
		Matrix4::Rotation(90, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-32800, -24000, -14000)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(90, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(41200, -24000, 23000)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(270, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(41200, -24000, 17500)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(270, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(24700, -35100, -50200)) * Matrix4::Scale(Vector3(400, 400, 400)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.283, 3, 0.354)) * Matrix4::Scale(Vector3(350, 400, 400)) * 
		Matrix4::Rotation(42, Vector3(0, 1, 0)) * Matrix4::Rotation(180, Vector3(1, 0, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.5, 1, 0.8) + Vector3(0, -600, 0)) * Matrix4::Scale(Vector3(150, 200, 250))
		* Matrix4::Rotation(45, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.673, 3.7, 0.883)) * Matrix4::Scale(Vector3(300, 600, 600))
		* Matrix4::Rotation(180, Vector3(1, 0, 0)) * Matrix4::Rotation(140, Vector3(0, 1, 0)));

	/* Textures */
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "concrete.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "concretebump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "water.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "rainsprite.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "building1tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "building2tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "signtex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "stubdot3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "advert1tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "roadtex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "roadbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "cartex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "carbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->sceneTextures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "advert2tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	int i = 0;
	for (auto& texture : resources->sceneTextures) {
		if (!texture)
			return;
		if(i == 4)
			SetTextureRepeating(texture, false);		// No repeat on rain
		else
			SetTextureRepeating(texture, true);
		++i;
	}
	ToggleAnisotropicFiltering(resources->sceneTextures);		// Default to 4x
	ToggleTrilinearFiltering(resources->sceneTextures);			// Trilinear on all textures, not bilinear

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "nx.png", TEXTUREDIR "px.png", TEXTUREDIR "py.png", 
		TEXTUREDIR "ny.png", TEXTUREDIR "nz.png", TEXTUREDIR "pz.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	if (!cubeMap)
		return;

	/* Lights */
	resources->lights.reserve(LIGHT_NUM);
	for (int i = 0; i < LIGHT_NUM; ++i) {
		switch (i) {
		case(0):
			resources->lights.push_back(new DirectionalLight(Vector3(-0.2f, -1.0f, -0.3f), Vector4(1, 1, 1, 1)));
			break;
		case(1):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.42f, 5.0f, 0.89f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(2):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.42f, 5.0f, 0.245f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(3):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.55f, 5.0f, 0.79f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(4):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.55f, 5.0f, 0.15f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(5):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.33f, 3.0f, 0.39f), Vector4(0, 1, 1, 1), 3000.0f));
			break;
		case(6):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.65f, 3.0f, 0.85f), Vector4(1, 0, 0, 1), 3000.0f));
			break;
		case(7):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.2, 12.0f, 0.2), Vector4(10, 0, 0, 1), 1000.0f));
			break;
		case(8):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.8f, 14.0f, 0.2f), Vector4(10, 0, 10, 1), 1000.0f));
			break;
		case(9):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.2f, 14.0f, 0.9f), Vector4(0, 0, 10, 1), 1000.0f));
			break;
		case(10):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.65f, 12.0f, 0.65f), Vector4(0, 10, 0, 1), 1000.0f));
			break;
		case(11):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.485f, 1.5f, 0.2f), Vector4(1, 1, 1, 1), Vector3(0, 0, 1), 3000.0f, 50.0f));
			break;
		case(12):
			resources->lights.push_back(new SpotLight(Vector3(0, 0, 0), Vector4(0, 0, 0, 1), Vector3(0, -1, 0), 6000.0f, 5.0f));
			break;
		}
	}

	/* Shadow buffers and FBOs */
	for (int i = 0; i < LIGHT_NUM; ++i) {
		glGenTextures(1, &shadowTex[i]);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &shadowFBO[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex[i], 0);
		glDrawBuffer(GL_NONE);
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !shadowTex[0]) 
		return;

	/* Post Processing textures and FBOs */
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glGenFramebuffers(1, &bufferFBO); 
	glGenFramebuffers(1, &processFBO); 
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0])
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	nr_particles = 1000;
	particles.reserve(nr_particles);
	
	rainInit = false;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	sceneTime = 0.0f;
	currentFrame = 0;
	animTime = 0.0f;
	brightness = 0.15f;
	waterCycle = 0.0f;
	dayTime = 0.1f;
	restTime = 10.0f;
	dayCycle = false;
	daySwap = true;
	resetCam = true;
	init = true;
	turning = false;
	forward = true;
	onRails = true;
	flashlight = false;
	currentEffect = 0;
}
Renderer::~Renderer(void) {
	glDeleteFramebuffers(LIGHT_NUM, shadowFBO);
	glDeleteTextures(LIGHT_NUM, shadowTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &processFBO);
	delete resources;
	delete heightMap;
}
void Renderer::UpdateScene(float dt) {
	if (dt < 0.5f) {		// Ensure frame is valid (long initial step)
		frameTime = dt;
		sceneTime += frameTime;
		waterCycle += frameTime * 0.25f;
		animTime -= dt;
		AnimateObjects();
		if (dayCycle)
			DayNightCycle();
		if (onRails) {
			if (resetCam) {		// Set camera back to beginning
				resources->cameras.at(0)->SetPosition(heightmapSize * Vector3(1, 30.0f, 1));
				resources->cameras.at(0)->SetYaw(45.0f);
				resources->cameras.at(0)->SetPitch(-45.0f);
				resources->cameras.at(0)->SetRoll(0.0f);
				sceneTime = 0.0f;
				resetCam = false;
			}
			TraverseScene();
		}
		else
			resources->cameras.at(0)->UpdateCamera(frameTime);

		resources->lights.at(0)->SetColour(Vector4(brightness, brightness, brightness, 1));		// Set global directional light
	}
}
void Renderer::TraverseScene() {
	std::cout << resources->cameras.at(0)->GetYaw() << std::endl;
	/* Camera on rails */
	if (sceneTime < 30.0f) {
		if ((int)resources->cameras.at(0)->GetYaw() != 0) {
			resources->cameras.at(0)->MoveRight(frameTime * 1.5);
			resources->cameras.at(0)->LookLeft(frameTime / 80);
		}
		if (sceneTime > 5.0) {
			if (currentEffect != 1)
				currentEffect = 1;		// Blur
		}
		if (sceneTime > 10) {
			if (currentEffect != 2)
				currentEffect = 2;		// Bloom
		}
		if (sceneTime > 15) {
			if (currentEffect != 3)
				currentEffect = 3;		// Color grading
		}
		if (sceneTime > 20) {
			if (currentEffect != 4)
				currentEffect = 4;		// Color concentrate
		}
		if (sceneTime > 25.0) 
				currentEffect = 0;
	}
	if ((int)resources->cameras.at(0)->GetYaw() == 0 && sceneTime < 33.0f) {
		dayCycle = false;
		if (abs((int)resources->cameras.at(0)->GetPosition().x - heightmapSize.x / 2) > 5)
			resources->cameras.at(0)->MoveRight(frameTime / 8);
		if ((int)resources->cameras.at(0)->GetPitch() < 0) 
			resources->cameras.at(0)->LookUp(frameTime / 100);
		if ((int)resources->cameras.at(0)->GetPosition().y > 500) 
			resources->cameras.at(0)->MoveDown(frameTime * 1.2);
	}
	if ((sceneTime > 33.0f && sceneTime < 36.0f) || (sceneTime > 42.0f && sceneTime < 44.0f)) 
		resources->cameras.at(0)->MoveForward(1.1 * frameTime);
	if (sceneTime > 36.0f && sceneTime < 42.0f) {
		if((int)resources->cameras.at(0)->GetYaw() != -90)
			resources->cameras.at(0)->SetYaw(-90);
		resources->cameras.at(0)->MoveLeft(frameTime / 2);
	}
	if (sceneTime > 42.0f && sceneTime < 44.0f) {
		resources->cameras.at(0)->SetYaw(0);
		if ((int)resources->cameras.at(0)->GetPitch() != -90)
			resources->cameras.at(0)->LookDown(frameTime / 20);
		if ((int)resources->cameras.at(0)->GetPosition().y < 6000)
			resources->cameras.at(0)->MoveUp(frameTime * 1.5);
	}
	if (sceneTime > 46.0f && sceneTime < 55.0f) 
		resources->cameras.at(0)->MoveBack(0.6 * frameTime);
	if (sceneTime > 55.0f && sceneTime < 60.0f) 
			resources->cameras.at(0)->MoveForward(0.8 * frameTime);
	if (sceneTime > 60.0f && sceneTime < 65.0f) {
		if ((int)resources->cameras.at(0)->GetPosition().y > 500)
			resources->cameras.at(0)->MoveDown(0.6 * frameTime);
		if ((int)resources->cameras.at(0)->GetYaw() > 270 || (int)resources->cameras.at(0)->GetYaw() < 90)
			resources->cameras.at(0)->LookLeft(frameTime / 20);
		if ((int)resources->cameras.at(0)->GetPitch() < 0)
			resources->cameras.at(0)->LookUp(frameTime / 20);
	}
	if (sceneTime > 65.0f && sceneTime < 71.0f) {
		if (resources->cameras.at(0)->GetPosition().x > 500)
			resources->cameras.at(0)->MoveBack(0.8 * frameTime);
	}
	if (sceneTime > 71.0f && sceneTime < 75.0f) {
		if (resources->cameras.at(0)->GetPosition().x > (heightmapSize.x / 2) + 500)
			resources->cameras.at(0)->MoveForward(0.8 * frameTime);
	}
	if (sceneTime > 75.0f && sceneTime < 80.0f) {
		if (resources->cameras.at(0)->GetYaw() < 180)
			resources->cameras.at(0)->LookLeft(frameTime / 40);
	}
	if (sceneTime > 80.0f && sceneTime < 94.0f) {
		resources->cameras.at(0)->MoveRight(frameTime * 0.45);
		resources->cameras.at(0)->LookLeft(frameTime / 80);
	}
	if (sceneTime > 94.0f) {
		if((int)resources->cameras.at(0)->GetPosition().x > heightmapSize.x / 2)
			resources->cameras.at(0)->MoveLeft(frameTime * 0.8);
		if ((int)resources->cameras.at(0)->GetPosition().y < heightmapSize.y * 50)
			resources->cameras.at(0)->MoveUp(frameTime * 0.8);
		if ((int)resources->cameras.at(0)->GetPosition().z > heightmapSize.z / 2)
			resources->cameras.at(0)->MoveForward(frameTime * 0.8);
		if ((int)resources->cameras.at(0)->GetPitch() > -90)
			resources->cameras.at(0)->LookDown(frameTime / 80);
		if ((int)resources->cameras.at(0)->GetYaw() > 0)
			resources->cameras.at(0)->LookLeft(frameTime / 80);
	}
	if (sceneTime > 98.0f)
		dayCycle = true;
}
void Renderer::AnimateObjects() {
	/* Move animated character, vehicle and moving lights */
	while (animTime < 0.0f) {
		currentFrame = (currentFrame + 1) % resources->sceneAnimations.at(0)->GetFrameCount();
		animTime += 1.0f / resources->sceneAnimations.at(0)->GetFrameRate();
	}
	if (fmod(sceneTime, 10) < frameTime && !turning) 
		turning = true;
	if (turning) {
		resources->sceneTransforms.at(6) = resources->sceneTransforms.at(6) * Matrix4::Rotation(90, Vector3(0, 1, 0));
		turning = !turning;
	}
	else
		resources->sceneTransforms.at(6) = resources->sceneTransforms.at(6) * Matrix4::Translation(Vector3(0, 0, 1.3 * frameTime));
	if (fmod(sceneTime, 11) < frameTime) 
		forward = !forward;
	if (forward) {
		SpotLight* s = dynamic_cast<SpotLight*> (resources->lights.at(11));
		resources->sceneTransforms.at(12) = resources->sceneTransforms.at(12) * Matrix4::Translation(Vector3(0, 0, 1.4 * frameTime));
		s->SetPosition(Matrix4::Translation(Vector3(0, 0, 9.2)) * s->GetPosition());
	}
	else {
		SpotLight* s = dynamic_cast<SpotLight*> (resources->lights.at(11));
		resources->sceneTransforms.at(12) = resources->sceneTransforms.at(12) * Matrix4::Translation(Vector3(0, 0, -1.4 * frameTime));
		s->SetPosition(Matrix4::Translation(Vector3(0, 0, -9.2)) * s->GetPosition());
	}

	/* Creating flashlight effect from spotlight */
	Matrix3 newMat = Matrix3(resources->cameras.at(0)->BuildViewMatrix());
	newMat.Transpose();
	Vector3 flashLight = newMat * Vector3(0, 0, -1);
	SpotLight* s = dynamic_cast<SpotLight*> (resources->lights.at(12));
	s->SetPosition(resources->cameras.at(0)->GetPosition());
	s->SetDirection(flashLight);
	if (flashlight) 
		resources->lights.at(12)->SetColour(Vector4(1, 1, 1, 1));
	else
		resources->lights.at(12)->SetColour(Vector4(0, 0, 0, 1));	// Invisible if deactivated
}
void Renderer::DayNightCycle() {
	if (restTime > 0.0f) {
		if (dayTime > 0.0 && daySwap) {
			brightness -= frameTime / 20;
			dayTime -= frameTime;
		}
		else if (dayTime < 10.0 && !daySwap) {
			brightness += frameTime / 20;
			dayTime += frameTime;
		}
		else
			restTime -= frameTime;
	}
	else {
		daySwap = dayTime > 0 ? true : false;
		restTime = 5.0f;		// 5 second daylight and night
	}
	if (brightness < 0.15)
		brightness = 0.15;
	if (brightness > 0.6)
		brightness = 0.6;
}
void Renderer::RenderScene() {
	/* Draw all for the first camera */
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawShadowScene();
	DrawSkybox();
	DrawMainScene(0);
	DrawRain();
	if (currentEffect != 0) {
		DrawPostProcess();
		PresentScene();
	}
	/* And only the main scene for the mini-map */
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0.75 * width, 0.66 * height, (width / height) * width / 3, (width / height) * height / 3);		// Minimap viewport
	DrawMainScene(1);
	glViewport(0, 0, width, height);		// Reset the skybox

	/* Filtering options */
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) 
		ToggleBilinearFiltering(resources->sceneTextures);
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) 
		ToggleTrilinearFiltering(resources->sceneTextures);
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
		ToggleAnisotropicFiltering(resources->sceneTextures);

	/* User Controls */
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_N)) {
		dayCycle = !dayCycle;
		dayCycle ? std::cout << "Day/Night Cycle On!" << std::endl : std::cout << "Day/Night Cycle Off!" << std::endl;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R)) {
		resetCam = true;
		onRails = !onRails;
		onRails ? std::cout << "Camera Rails On!" << std::endl : std::cout << "Camera Rails Off!" << std::endl;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		if (currentEffect == 4)
			currentEffect = 0;
		else 
			currentEffect++;
		//postProcess ? std::cout << "Post Processing On!"  << std::endl : std::cout <<"Post Processing Off!" << std::endl;
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F)) {
		flashlight = !flashlight;
		flashlight ? std::cout << "Flashlight On!" << std::endl : std::cout << "Flashlight Off!" << std::endl;
	}
}
void Renderer::DrawSkybox() {
	if (currentEffect != 0) {		// Bind our buffers here if post processing operations
		glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	glDepthMask(GL_FALSE);
	BindShader(resources->sceneShaders.at(0));
	viewMatrix = resources->cameras.at(0)->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(50.0f, 20000.0f, (float)width / (float)height, 45);
	UpdateShaderMatrices();
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "brightness"), brightness);
	resources->sceneMeshes.at(3)->Draw();
	glDepthMask(GL_TRUE);
}
void Renderer::DrawShadowScene() {
	BindShader(resources->sceneShaders.at(1));
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), 0.0);
	for (int i = 0; i < LIGHT_NUM; ++i) {		// Draw a shadow map for each of our lights 
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);

		/* Binding shadow projection */
		if (PointLight* p = dynamic_cast<PointLight*> (resources->lights.at(i))) 
			viewMatrix = Matrix4::BuildViewMatrix(p->GetPosition(), heightmapSize * Vector3(0.5, 0, 0.5), Vector3(1, 0, 0));
		else if (SpotLight* s = dynamic_cast<SpotLight*> (resources->lights.at(i))) 
			viewMatrix = Matrix4::BuildViewMatrix(s->GetPosition(), s->GetPosition() + s->GetDirection(), Vector3(1, 0, 0));
		else if (DirectionalLight* d = dynamic_cast<DirectionalLight*> (resources->lights.at(i)))
			viewMatrix = Matrix4::BuildViewMatrix(Vector3(heightmapSize.x / 2 - 0.1, 1000, heightmapSize.z / 2), 
				Vector3(heightmapSize.x / 2, 0, heightmapSize.z / 2), Vector3(1, 0, 0));
		projMatrix = Matrix4::Perspective(50.0f, 20000.0f, 1.0f, 120.0f);
		shadowMatrices[i] = projMatrix * viewMatrix; 

		/* HeightMap */
		modelMatrix.ToIdentity();
		UpdateShaderMatrices();
		heightMap->Draw();

		/* Water */
		modelMatrix = resources->sceneTransforms.at(5);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(3)->Draw();

		/* Animated Mesh */
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), true);
		modelMatrix = resources->sceneTransforms.at(6);
		UpdateShaderMatrices();
		vector <Matrix4> frameMatrices;
		const Matrix4* invBindPose = resources->sceneMeshes.at(4)->GetInverseBindPose();
		const Matrix4* frameData = resources->sceneAnimations.at(0)->GetJointData(currentFrame);
		for (unsigned int i = 0; i < resources->sceneMeshes.at(4)->GetJointCount(); ++i)
			frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
		int j = glGetUniformLocation(currentShader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
		for (int i = 0; i < resources->sceneMeshes.at(4)->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, resources->matTextures.at(0)[i]);
			resources->sceneMeshes.at(4)->DrawSubMesh(i);
		}
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), false);

		/* Building 1 */
		modelMatrix = resources->sceneTransforms.at(0);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(5)->Draw();

		modelMatrix = resources->sceneTransforms.at(1);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(5)->Draw();

		/* Buildings 2 */
		modelMatrix = resources->sceneTransforms.at(2);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(6)->Draw();

		modelMatrix = resources->sceneTransforms.at(3);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(6)->Draw();

		/* Sign 1 */
		modelMatrix = resources->sceneTransforms.at(4);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(7)->Draw();

		modelMatrix = resources->sceneTransforms.at(13);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(3)->Draw();

		/* Sign 2 */
		modelMatrix = resources->sceneTransforms.at(14);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(7)->Draw();

		modelMatrix = resources->sceneTransforms.at(15);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(3)->Draw();

		/* Road */
		modelMatrix = resources->sceneTransforms.at(7);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(9)->Draw();

		/* Lights */
		modelMatrix = resources->sceneTransforms.at(8);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(10)->Draw();

		modelMatrix = resources->sceneTransforms.at(9);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(10)->Draw();

		modelMatrix = resources->sceneTransforms.at(10);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(10)->Draw();

		modelMatrix = resources->sceneTransforms.at(11);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(10)->Draw();

		/* Car */
		modelMatrix = resources->sceneTransforms.at(12);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(11)->Draw();
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawMainScene(int camera) {
	BindShader(resources->sceneShaders.at(2));
	SetShaderLights(resources->lights);		// Array uniform setting function in OGL renderer
	modelMatrix.ToIdentity();
	viewMatrix = resources->cameras.at(camera)->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(50.0f, 20000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	/* Create our array of shadow textures and send array to shader */
	int arr[LIGHT_NUM] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	glUniform1iv(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), LIGHT_NUM, (GLint*)arr);
	for (int i = 0; i < LIGHT_NUM; i++) {
		glActiveTexture(GL_TEXTURE3 + i);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
	}

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "shine"), 100.0f);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "iridescence"), 1.0f);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&resources->cameras.at(0)->GetPosition());
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrices"), LIGHT_NUM, false, (float*)&shadowMatrices);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), false);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "reflected"), false);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "signMove"), false);

	/* HeightMap */
	SetTexture(resources->sceneTextures.at(0), 0, "diffuseTex", currentShader);
	SetTexture(resources->sceneTextures.at(1), 1, "bumpTex", currentShader);
	heightMap->Draw();
	
	/* Animated Mesh */
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), true);
	modelMatrix = resources->sceneTransforms.at(6);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	vector <Matrix4> frameMatrices;
	const Matrix4* invBindPose = resources->sceneMeshes.at(4)->GetInverseBindPose();
	const Matrix4* frameData = resources->sceneAnimations.at(0)->GetJointData(currentFrame);

	for (unsigned int i = 0; i < resources->sceneMeshes.at(4)->GetJointCount(); ++i)
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);

	int j = glGetUniformLocation(currentShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	for (int i = 0; i < resources->sceneMeshes.at(4)->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, resources->matTextures.at(0)[i]);
		resources->sceneMeshes.at(4)->DrawSubMesh(i);
	}
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "animated"), false);

	/* Buildings 1 */
	SetTexture(resources->sceneTextures.at(5), 0, "diffuseTex", currentShader);
	SetTexture(resources->sceneTextures.at(8), 1, "bumpTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(0);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(5)->Draw();

	modelMatrix = resources->sceneTransforms.at(1);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(5)->Draw();

	/* Water */
	SetTexture(resources->sceneTextures.at(2), 0, "diffuseTex", currentShader);
	SetTexture(resources->sceneTextures.at(8), 1, "bumpTex", currentShader);		// Dot3 on water
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "reflected"), true);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "reflectBrightness"), brightness);
	modelMatrix = resources->sceneTransforms.at(5);
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10));
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
	textureMatrix.ToIdentity();
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "reflected"), false);

	/* Buildings 2 */
	SetTexture(resources->sceneTextures.at(6), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(2);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(6)->Draw();

	modelMatrix = resources->sceneTransforms.at(3);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(6)->Draw();

	/* Sign 1 */
	SetTexture(resources->sceneTextures.at(7), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(4);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(7)->Draw();

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "signMove"), true);
	SetTexture(resources->sceneTextures.at(9), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(13);
	textureMatrix = Matrix4::Translation(Vector3(0.0f, waterCycle / 2, 0.0f)) * Matrix4::Scale(Vector3(1, 0.5, 1));
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
	textureMatrix.ToIdentity();
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "signMove"), false);

	/* Sign 2 */
	SetTexture(resources->sceneTextures.at(7), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(14);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(7)->Draw();

	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "signMove"), true);
	SetTexture(resources->sceneTextures.at(14), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(15);
	textureMatrix = Matrix4::Translation(Vector3(waterCycle / 2, 0.0f, waterCycle / 2)) * Matrix4::Scale(Vector3(0.5, 1, 0.5));
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
	textureMatrix.ToIdentity();
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "signMove"), false);

	/* Road */
	SetTexture(resources->sceneTextures.at(10), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(7);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(9)->Draw();

	/* Lights */
	SetTexture(resources->sceneTextures.at(7), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(8);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(10)->Draw();

	modelMatrix = resources->sceneTransforms.at(9);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(10)->Draw();

	modelMatrix = resources->sceneTransforms.at(10);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(10)->Draw();

	modelMatrix = resources->sceneTransforms.at(11);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(10)->Draw();

	/* Car */
	SetTexture(resources->sceneTextures.at(12), 0, "diffuseTex", currentShader);
	SetTexture(resources->sceneTextures.at(8), 1, "bumpTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(12);		
	UpdateShaderMatrices();
	resources->sceneMeshes.at(11)->Draw();
}
void Renderer::DrawRain() {
	BindShader(resources->sceneShaders.at(3));
	SetTexture(resources->sceneTextures.at(4), 0, "diffuseTex", currentShader);
	for (int i = 0; i < nr_particles; ++i) {
		viewMatrix = resources->cameras.at(0)->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(50.0f, 20000.0f, (float)width / (float)height, 45);
		Vector3 randLoc(rand() % (int)heightmapSize.x, rand() % 10000 + 6000, rand() % (int)heightmapSize.z);
		if (!rainInit) 
			particles.push_back(Particle(resources->sceneMeshes.at(3), randLoc));
		else 
			particles.at(i).SetPosition(particles.at(i).GetPosition() + Vector3(0, -10000 * frameTime, 0));
		if (particles.at(i).GetPosition().y < -10)		// Move rain back to top of screen
			particles.at(i).SetPosition(randLoc);
		modelMatrix = Matrix4::Translation(particles.at(i).GetPosition()) * Matrix4::Scale(Vector3(7, rand()% 500 + 100, 7));
		UpdateShaderMatrices();
		particles.at(i).GetMesh()->Draw();
	}
	rainInit = true;
	if(currentEffect != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);		// And unbind our depth buffer here
}
void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	switch (currentEffect) {
	case(1):
		BindShader(resources->sceneShaders.at(4));		// Blur processing
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();
		projMatrix.ToIdentity();
		textureMatrix.ToIdentity();
		UpdateShaderMatrices();
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "sceneTex"), 0);

		for (int i = 0; i < POST_PASSES; ++i) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);
			glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
			resources->sceneMeshes.at(3)->Draw();

			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
			glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
			resources->sceneMeshes.at(3)->Draw();
		}
		break;
	case(2):
		BindShader(resources->sceneShaders.at(5));		// Bloom processing
		break;
	case(3):
		BindShader(resources->sceneShaders.at(6));		// Bloom processing
		break;
	case(4):
		BindShader(resources->sceneShaders.at(7));		// Sceen wave
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "offset"), waterCycle * 20);
		break;
	}
	if (currentEffect != 1) {
		modelMatrix.ToIdentity();
		viewMatrix.ToIdentity();
		projMatrix.ToIdentity();
		textureMatrix.ToIdentity();
		UpdateShaderMatrices();
		glDisable(GL_DEPTH_TEST);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "sceneTex"), 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
		resources->sceneMeshes.at(3)->Draw();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[1]);
		resources->sceneMeshes.at(3)->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}
void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(resources->sceneShaders.at(8));
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	resources->sceneMeshes.at(3)->Draw();		// Draw textured quad back to screen
}
bool Renderer::SetTexture(GLuint texID, GLuint unit, const std::string& uniformName, Shader* s) {
	GLint uniformID = glGetUniformLocation(s->GetProgram(), uniformName.c_str());
	if (uniformID < 0) {
		std::cout << "Trying to bind invalid 2D texture uniform!\n"; 
		return false;
	}
	if (currentShader != s) {
		std::cout << "Trying to set shader uniform on wrong shader!\n"; 
		return false;
	}
	glActiveTexture(GL_TEXTURE0 + unit); 
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(uniformID, unit);
	return true;
}