/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069
 *	Main class for scene graph usage */
#include "../nclgl/window.h"
#include "Renderer.h"

int main() {	
	Window w("Scene Management!", 1280, 720,false);
	if(!w.HasInitialised()) 
		return -1;

	Renderer renderer(w);
	if(!renderer.HasInitialised()) 
		return -1;

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		float timestep = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(timestep);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) 
			Shader::ReloadAllShaders();
	}
	return 0;
}