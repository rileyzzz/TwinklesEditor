#pragma once
#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Shader.h"
#include "TwinklesSystem.h"
//#include <SDL_ttf.h>
//class TwinklesSystem;

//extern TwinklesSystem* ActiveSystem;

struct RenderParticle
{
	glm::vec3 Position;

	glm::vec3 Velocity;

	glm::vec4 Color;

	float Size;

	float Life;

	float Rotation;

	//Not sent to particle shader, initial properties at time of emission
	//float Rand; //per particle random

	float RotationRate;

	float Lifetime;

	float sizeVariance;

	float gravity;

	glm::vec2 MinMax;
	//glm::mat4 Transform;

	//Position(0.0f),
	RenderParticle() : Velocity(0.0f), Color(1.0f), Size(1.0f), Life(0.0f), Rotation(0.0f), RotationRate(0.0f), Lifetime(0.0f), sizeVariance(0.0f), gravity(0.0f), MinMax(0.0f) {}
};

class Scene;

class RenderEmitter
{
private:
	Emitter& SourceEmitter;
	const uint32_t particleCount = 10000;
	std::vector<RenderParticle> particles;

	bool EmitterPaused = true;
	float EmitterLife = 1.0f;
	const float EmitterLifetime = 1.0f;

	float particleAccumulator = 0.0f;
	uint32_t FirstUnusedParticle();
	void ParticleRespawn(RenderParticle& particle);
	Shader* particleShader = nullptr;

	uint32_t EmitterVBO;
	uint32_t EmitterVAO;

	bool TextureLoaded = false;
	uint32_t SpriteTex;

	Scene* parentScene;
public:
	void SetTexture(const char* path);
	void EmitterTick(float deltaTime);
	void DrawParticles();
	RenderEmitter(Emitter& emit, Scene* InScene);
};

class Scene
{
public:
	unsigned int texColorBuffer = 0;
	bool CaptureMouse = false;

	std::string Directory;
private:
	//glm::mat4 cameraView;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 cameraRot = glm::vec3(-75.0f, 0.0f, 15.0f);

	int width = 1400;
	int height = 1024;
	unsigned int fbo;
	unsigned int rbo;

	double zoom = 4.0;

public:
	TwinklesSystem ActiveSystem;
	std::vector<RenderEmitter> Emitters;

	Shader* particleShader = nullptr;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;

	void Keyboard(SDL_KeyboardEvent keyevent, bool state);
	void MouseWheel(SDL_MouseWheelEvent wheelevent);
	void MouseMove(SDL_MouseMotionEvent motionevent);
	void Mouse(SDL_MouseButtonEvent buttonevent, bool state);
	

	//void ProcessEvent(const SDL_Event* event);
	Scene(std::string InDirectory);
	~Scene();

	void InitGL();
	void ResizeScene(int inWidth, int inHeight);
	void Render(float deltaTime);

	void OpenFile(const char* path);
	void ExportFile(const char* path);
};

