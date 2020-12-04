/*          Created By Samuel Buzz Appleby
 *               03/12/2020
 *                170348069                  
 *	Main function, create window and renderer	 */
#include "Renderer.h"
#include "../nclgl/Window.h"

int main() {
	Window w("CyberPunk", 1920, 1080, true); 
	if (!w.HasInitialised()) 
		return -1;
	Renderer renderer(w); 
	if (!renderer.HasInitialised()) 
		return -1;
	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) 
			Shader::ReloadAllShaders();
	}
	return 0;
}