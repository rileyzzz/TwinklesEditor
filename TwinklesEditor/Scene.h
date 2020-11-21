#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <SDL_ttf.h>

class Scene
{
private:
	void InitGL();
	//glm::mat4 cameraView;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraRot = glm::vec3(-75.0f, 0.0f, 15.0f);

	int width, height;
	unsigned int fbo;
	unsigned int rbo;

	double zoom = 4.0;



public:
	unsigned int texColorBuffer;

	void Keyboard(SDL_KeyboardEvent keyevent, bool state);
	void MouseWheel(SDL_MouseWheelEvent wheelevent);
	void MouseMove(SDL_MouseMotionEvent motionevent);
	void Mouse(SDL_MouseButtonEvent buttonevent, bool state);
	bool CaptureMouse = false;
	//void ProcessEvent(const SDL_Event* event);
	Scene()
	{
		InitGL();
	}
	~Scene();
	void ResizeScene(int inWidth, int inHeight);
	void Render();
};

