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
#include "TwinklesSystem.h"
//#include <SDL_ttf.h>
//class TwinklesSystem;

//extern TwinklesSystem* ActiveSystem;

struct RenderParticle
{
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float Life;
	RenderParticle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class RenderEmitter
{
private:
	Emitter& SourceEmitter;
	uint32_t particleCount = 10000;
	std::vector<RenderParticle> particles;
	uint32_t FirstUnusedParticle();
	void ParticleRespawn(RenderParticle& particle);



public:
	void EmitterTick(float deltaTime);
	void DrawParticles();
	RenderEmitter(Emitter& emit) : SourceEmitter(emit)
	{
		for (unsigned int i = 0; i < particleCount; ++i)
			particles.push_back(RenderParticle());

		//std::cout << "Color along:\n";
		//for (float i = 0.0f; i < 1.0f; i += 0.1f)
		//{
		//	Color color = SourceEmitter.Color.GetKey(i);
		//	std::cout << color << "\n";
		//	//std::cout << "color " << particles[0].Color.r << " " << particles[0].Color.g << " " << particles[0].Color.b << " " << particles[0].Color.a << "\n";
		//}
	}
};

class Scene
{
public:
	unsigned int texColorBuffer = 0;
	bool CaptureMouse = false;

private:
	
	//glm::mat4 cameraView;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraRot = glm::vec3(-75.0f, 0.0f, 15.0f);

	int width, height;
	unsigned int fbo;
	unsigned int rbo;

	double zoom = 4.0;



	TwinklesSystem ActiveSystem;
	std::vector<RenderEmitter> Emitters;
public:
	

	void Keyboard(SDL_KeyboardEvent keyevent, bool state);
	void MouseWheel(SDL_MouseWheelEvent wheelevent);
	void MouseMove(SDL_MouseMotionEvent motionevent);
	void Mouse(SDL_MouseButtonEvent buttonevent, bool state);
	

	//void ProcessEvent(const SDL_Event* event);
	Scene()
	{

	}
	~Scene();

	void InitGL();
	void ResizeScene(int inWidth, int inHeight);
	void Render(float deltaTime);

	void OpenFile(const char* path);
	void ExportFile(const char* path);
};

