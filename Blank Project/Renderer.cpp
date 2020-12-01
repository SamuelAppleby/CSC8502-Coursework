#include  "Renderer.h"
#include "../nclgl/Camera.h"
#define SHADOWSIZE 4056
#include <algorithm>
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	resources = new ResourceManager();
	heightMap = new HeightMap(TEXTUREDIR "rivermap.png");
	heightmapSize = heightMap->GetHeightmapSize();
	resources->cameras.push_back(new Camera(0.0f, 0.0f, 0.0f, heightmapSize * Vector3(0.5, 2.0f,1)));
	resources->cameras.push_back(new Camera(-90, 0, 0.0f, heightmapSize * Vector3(0.5, 50.0f, 0.5)));

	/* Shaders */
	resources->shaders.push_back(new Shader("SkyboxVertex.glsl", "SkyboxFragment.glsl"));
	resources->shaders.push_back(new Shader("shadowVertex.glsl", "shadowFragment.glsl"));
	resources->shaders.push_back(new Shader("shadowscenevertex.glsl", "LightingFragment.glsl"));
	resources->shaders.push_back(new Shader("ReflectBumpVertex.glsl", "ReflectBumpFragment.glsl"));
	resources->shaders.push_back(new Shader("RainVertex.glsl", "RainFragment.glsl"));
	resources->shaders.push_back(new Shader("SkinningVertex.glsl", "texturedFragment.glsl"));
	for(auto& shader : resources->shaders)
		if (!shader->LoadSuccess()) 
		return;

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

	/* Materials */
	resources->sceneMaterials.push_back(new MeshMaterial("Role_T.mat"));
	resources->sceneMaterials.push_back(new MeshMaterial("b1.mat"));
	resources->sceneMaterials.push_back(new MeshMaterial("b2.mat"));
	resources->sceneMaterials.push_back(new MeshMaterial("portrait_board.mat"));

	/* Animations */
	resources->sceneAnimations.push_back(new MeshAnimation("Role_T.anm"));

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
	resources->matTextures.push_back(newMaterials);
	for (int i = 0; i < resources->sceneMeshes.at(5)->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = resources->sceneMaterials.at(1)->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		resources->matTextures.at(1).emplace_back(texID);
	}
	resources->matTextures.push_back(newMaterials);
	for (int i = 0; i < resources->sceneMeshes.at(6)->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = resources->sceneMaterials.at(2)->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		resources->matTextures.at(2).emplace_back(texID);
	}
	resources->matTextures.push_back(newMaterials);
	for (int i = 0; i < resources->sceneMeshes.at(7)->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = resources->sceneMaterials.at(3)->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		resources->matTextures.at(3).emplace_back(texID);
	}
	/* Scene Transforms */
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.1, 0, 0.1) + Vector3(0, -2000, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(-0.4, 0, -0.4) + Vector3(0, -2000, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(-0.3f, 0, 0.2) + Vector3(0, -1600, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.3, 0, -0.6) + Vector3(0, -1600, 0)) * Matrix4::Scale(Vector3(500, 500, 500)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.1, 1, 0.3) + Vector3(0, -500, 0)) * Matrix4::Rotation(45, Vector3(0, 1, 0)) 
		* Matrix4::Scale(Vector3(150, 150, 150)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * 0.5f) * Matrix4::Scale(heightmapSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.77, 0.8, 0.6)) * Matrix4::Scale(Vector3(100, 100, 100)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-600, -25780, -69000)) * Matrix4::Scale(Vector3(80, 150, 300)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-32800, -24000, -8500)) * Matrix4::Scale(Vector3(300, 300, 300)) * 
		Matrix4::Rotation(90, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(-32800, -24000, -14000)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(90, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(41200, -24000, 23000)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(270, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(41200, -24000, 17500)) * Matrix4::Scale(Vector3(300, 300, 300)) *
		Matrix4::Rotation(270, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(Vector3(24700, -35100, -50000)) * Matrix4::Scale(Vector3(400, 400, 400)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.282, 3, 0.355)) * Matrix4::Scale(Vector3(350, 400, 400)) * 
		Matrix4::Rotation(42, Vector3(0, 1, 0)) * Matrix4::Rotation(180, Vector3(1, 0, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.5, 1, 0.8) + Vector3(0, -600, 0)) * Matrix4::Scale(Vector3(150, 200, 250))
		* Matrix4::Rotation(45, Vector3(0, 1, 0)));
	resources->sceneTransforms.push_back(Matrix4::Translation(heightmapSize * Vector3(0.673, 3.7, 0.883)) * Matrix4::Scale(Vector3(300, 600, 600))
		* Matrix4::Rotation(180, Vector3(1, 0, 0)) * Matrix4::Rotation(140, Vector3(0, 1, 0)));

	/* Textures */
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "concrete.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "concretebump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "water.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "rainsprite.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "building1tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "building2tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "signtex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "stubdot3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "adverttex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "roadtex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "roadbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "cartex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "carbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	resources->textures.push_back(SOIL_load_OGL_texture(TEXTUREDIR "advert2tex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	int i = 0;
	for (auto& texture : resources->textures) {
		if (!texture)
			return;
		if(i == 4)
			SetTextureRepeating(texture, false);
		else
			SetTextureRepeating(texture, true);
		++i;
	}
	ToggleAnisotropicFiltering(resources->textures);
	ToggleTrilinearFiltering(resources->textures);

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
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.42f, 5.0f, 0.9f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(2):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.42f, 5.0f, 0.25f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(3):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.55f, 5.0f, 0.8f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(4):
			resources->lights.push_back(new SpotLight(heightmapSize * Vector3(0.55f, 5.0f, 0.17f), Vector4(1, 1, 0, 1), Vector3(0, -1, 0), 2000.0f, 20.0f));
			break;
		case(5):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.33f, 3.0f, 0.39f), Vector4(0, 1, 1, 1), 3000.0f));
			break;
		case(6):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.65f, 3.0f, 0.85f), Vector4(1, 0, 0, 1), 3000.0f));
			break;
		case(7):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.2, 12.0f, 0.2), Vector4(1, 0, 0, 1), 3000.0f));
			break;
		case(8):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.8f, 14.0f, 0.2f), Vector4(1, 0, 1, 1), 3000.0f));
			break;
		case(9):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.2f, 14.0f, 0.9f), Vector4(0, 0, 1, 1), 3000.0f));
			break;
		case(10):
			resources->lights.push_back(new PointLight(heightmapSize * Vector3(0.6f, 12.0f, 0.6f), Vector4(0, 1, 0, 1), 3000.0f));
			break;
		}
	}

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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	sceneTime = 0.0f;
	currentFrame = 0;
	animTime = 0.0f;
	brightness = 0.2f;
	waterCycle = 0.0f;
	dayTime = 0.1f;
	restTime = 10.0f;
	daySwap = true;
	init = true;
	rainInit = false;
	turnTimer = 0.0f;
	carTimer = 0.0f;
	straight = true;
	particles.reserve(nr_particles);
}
Renderer::~Renderer(void) {
	glDeleteTextures(LIGHT_NUM, shadowTex);
	glDeleteFramebuffers(LIGHT_NUM, shadowFBO);
	
	delete heightMap;
}
void Renderer::UpdateScene(float dt) {
	frameTime = dt;
	resources->cameras.at(0)->UpdateCamera(frameTime);
	sceneTime += frameTime;
	waterCycle += frameTime * 0.25f; // 10 units a second
	DayNightCycle(frameTime);
	animTime -= dt;
	while (animTime < 0.0f) {
		currentFrame = (currentFrame + 1) % resources->sceneAnimations.at(0)->GetFrameCount();
		animTime += 1.0f / resources->sceneAnimations.at(0)->GetFrameRate();
	}
	turnTimer += dt;
	if(rainInit)
		carTimer += dt;
	if (turnTimer > 10.0f) 
		turnTimer = 0.0f;
	if (turnTimer == 0.0f){
		if(!rainInit)
			resources->sceneTransforms.at(6) = resources->sceneTransforms.at(6) * Matrix4::Rotation(-90, Vector3(0, 1, 0));
		else
			resources->sceneTransforms.at(6) = resources->sceneTransforms.at(6) * Matrix4::Rotation(90, Vector3(0, 1, 0));
	}
	else 
		resources->sceneTransforms.at(6) = resources->sceneTransforms.at(6) * Matrix4::Translation(Vector3(0, 0, 1.3 * dt));
	if (carTimer > 18.0f) {
		straight = !straight;
		carTimer = 0.0f;
	}
	if (carTimer > 0.0f) {
		if (straight)
			resources->sceneTransforms.at(12) = resources->sceneTransforms.at(12) * Matrix4::Translation(Vector3(0, 0, dt));
		else
			resources->sceneTransforms.at(12) = resources->sceneTransforms.at(12) * Matrix4::Translation(Vector3(0, 0, -dt));
	}
}
void Renderer::DayNightCycle(float frameTime) {
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
		restTime = 5.0f;
	}
	if (brightness < 0.2)
		brightness = 0.2;
	if (brightness > 0.6)
		brightness = 0.6;
	resources->lights.at(0)->SetColour(Vector4(0.2, 0.2, 0.2, 1));
}
void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkybox(0);
	DrawShadowScene();
	DrawMainScene(0);
	DrawWater(0);
	DrawAnimation(0);
	DrawRain(frameTime);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(-100, (2 * height) / 3, width / 3, height / 3);
	DrawMainScene(1);
	DrawWater(1);
	DrawAnimation(1);
	glViewport(0, 0, width, height);

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
		ToggleBilinearFiltering(resources->textures);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
		ToggleTrilinearFiltering(resources->textures);
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
		ToggleAnisotropicFiltering(resources->textures);
	}
}
void Renderer::DrawSkybox(int camera) {
	glDepthMask(GL_FALSE);
	BindShader(resources->shaders.at(0));
	viewMatrix = resources->cameras.at(camera)->BuildViewMatrix();
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "brightness"), brightness);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
	glDepthMask(GL_TRUE);
}
void Renderer::DrawShadowScene() {
	BindShader(resources->shaders.at(1));
	for (int i = 0; i < LIGHT_NUM; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[i]);
		if (PointLight* p = dynamic_cast<PointLight*> (resources->lights.at(i))) 
			viewMatrix = Matrix4::BuildViewMatrix(p->GetPosition(), heightmapSize * Vector3(0.5, 0, 0.5), Vector3(1, 0, 0));
		else if (SpotLight* s = dynamic_cast<SpotLight*> (resources->lights.at(i))) 
			viewMatrix = Matrix4::BuildViewMatrix(s->GetPosition(), s->GetPosition() + s->GetDirection(), Vector3(1, 0, 0));
		else if (DirectionalLight* d = dynamic_cast<DirectionalLight*> (resources->lights.at(i)))
			viewMatrix = Matrix4::BuildViewMatrix(Vector3(heightmapSize.x / 2 - 0.1, 1000, heightmapSize.z / 2), 
				Vector3(heightmapSize.x / 2, 0, heightmapSize.z / 2), Vector3(1, 0, 0));
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 120);
		shadowMatrices[i] = projMatrix * viewMatrix; 
		modelMatrix.ToIdentity();
		UpdateShaderMatrices();
		heightMap->Draw();

		modelMatrix = resources->sceneTransforms.at(6);
		UpdateShaderMatrices();
		for (int i = 0; i < resources->sceneMeshes.at(4)->GetSubMeshCount(); ++i) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, resources->matTextures.at(0)[i]);
			resources->sceneMeshes.at(4)->DrawSubMesh(i);
		}

		modelMatrix = resources->sceneTransforms.at(0);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(5)->Draw();

		modelMatrix = resources->sceneTransforms.at(1);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(5)->Draw();

		modelMatrix = resources->sceneTransforms.at(2);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(6)->Draw();

		modelMatrix = resources->sceneTransforms.at(3);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(6)->Draw();

		modelMatrix = resources->sceneTransforms.at(4);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(7)->Draw();

		modelMatrix = resources->sceneTransforms.at(7);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(9)->Draw();

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

		modelMatrix = resources->sceneTransforms.at(12);
		UpdateShaderMatrices();
		resources->sceneMeshes.at(11)->Draw();
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawMainScene(int camera) {
	BindShader(resources->shaders.at(2));
	SetShaderLights(resources->lights);
	modelMatrix.ToIdentity();
	viewMatrix = resources->cameras.at(camera)->BuildViewMatrix();
	if(camera == 0)
		projMatrix = Matrix4::Perspective(1.0f, 150000.0f, (float)width / (float)height, 45.0f);
	else
		projMatrix = Matrix4::Perspective(1.0f, 150000.0f, (float)width / (float)height, 45.0f);
	UpdateShaderMatrices();

	int arr[LIGHT_NUM] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
	glUniform1iv(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), LIGHT_NUM, (GLint*)arr);
	for (int i = 0; i < LIGHT_NUM; i++) {
		glActiveTexture(GL_TEXTURE2 + i);
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
	}
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "shine"), 100.0f);
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "iridescence"), 1.0f);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&resources->cameras.at(0)->GetPosition());
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "shadowMatrices"), LIGHT_NUM, false, (float*)&shadowMatrices);

	SetTexture(resources->textures.at(0), 0, "diffuseTex", currentShader);
	SetTexture(resources->textures.at(1), 1, "bumpTex", currentShader);
	heightMap->Draw();
	
	/* Buildings 1 */
	SetTexture(resources->textures.at(5), 0, "diffuseTex", currentShader);
	SetTexture(resources->textures.at(8), 1, "bumpTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(0);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(5)->Draw();

	modelMatrix = resources->sceneTransforms.at(1);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(5)->Draw();

	/* Buildings 2 */
	SetTexture(resources->textures.at(6), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(2);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(6)->Draw();

	modelMatrix = resources->sceneTransforms.at(3);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(6)->Draw();

	/* Sign */
	SetTexture(resources->textures.at(7), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(4);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(7)->Draw();

	SetTexture(resources->textures.at(9), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(13);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();

	/* Road */
	SetTexture(resources->textures.at(10), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(7);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(9)->Draw();

	/* Lights */
	SetTexture(resources->textures.at(7), 0, "diffuseTex", currentShader);
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
	SetTexture(resources->textures.at(12), 0, "diffuseTex", currentShader);
	SetTexture(resources->textures.at(13), 1, "bumpTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(12);		
	UpdateShaderMatrices();
	resources->sceneMeshes.at(11)->Draw();

	/* Sign 2 */
	SetTexture(resources->textures.at(7), 0, "diffuseTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(14);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(7)->Draw();

	SetTexture(resources->textures.at(14), 0, "diffuseTex", currentShader);
	SetTexture(resources->textures.at(8), 1, "bumpTex", currentShader);
	modelMatrix = resources->sceneTransforms.at(15);
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
}
void Renderer::DrawWater(int camera) {
	BindShader(resources->shaders.at(3));
	SetTexture(resources->textures.at(2), 0, "diffuseTex", currentShader);
	SetTexture(resources->textures.at(8), 1, "bumpTex", currentShader);		// Dot3 on water

	glUniform1i(glGetUniformLocation(resources->shaders.at(3)->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&resources->cameras.at(0)->GetPosition());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "reflectBrightness"), brightness);

	modelMatrix = resources->sceneTransforms.at(5);
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10));
	UpdateShaderMatrices();
	resources->sceneMeshes.at(3)->Draw();
}
void Renderer::DrawAnimation(int camera) {
	BindShader(resources->shaders.at(5));
	modelMatrix = resources->sceneTransforms.at(6);
	viewMatrix = resources->cameras.at(camera)->BuildViewMatrix();
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
}
void Renderer::DrawRain(float frameTime) {
	BindShader(resources->shaders.at(4));
	SetTexture(resources->textures.at(4), 0, "diffuseTex", currentShader);
	for (int i = 0; i < nr_particles; ++i) {
		viewMatrix = resources->cameras.at(0)->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 150000.0f, (float)width / (float)height, 45);
		Vector3 randLoc(rand() % (int)heightmapSize.x, rand() % 10000 + 6000, rand() % (int)heightmapSize.z);
		if (!rainInit) {
			particles.push_back(Particle(resources->sceneMeshes.at(3), randLoc));
			modelMatrix = Matrix4::Translation(randLoc) * Matrix4::Scale(Vector3(10, 10, 10));
		}
		else {
			particles.at(i).SetPosition(particles.at(i).GetPosition() + Vector3(0, -5000 * frameTime, 0));
			if (particles.at(i).GetPosition().y < -10)
				particles.at(i).SetPosition(randLoc);
			modelMatrix = Matrix4::Translation(particles.at(i).GetPosition()) * Matrix4::Scale(Vector3(20, 20, 20));
		}
		UpdateShaderMatrices();
		particles.at(i).GetMesh()->Draw();
	}
	rainInit = true;
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