#pragma once
#include <map>
#include <filesystem>
#define NOMINMAX
#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Scene.h"
#include "TwinklesSystem.h"

static uint32_t ObjectID = 0;

struct EditEmitter
{
	uint32_t ID = 0;
	Emitter* SourceEmitter;
	std::map<std::string, KeyframeTrackBase*> Tracks;

	EditEmitter(Emitter* InSourceEmitter) : ID(ObjectID++), SourceEmitter(InSourceEmitter)
	{
		Tracks["Emitter Size"] = &SourceEmitter->EmitterSize;
		Tracks["Emission Rate"] = &SourceEmitter->EmissionRate;
		Tracks["Velocity Cone"] = &SourceEmitter->VelocityCone;
		Tracks["Z Speed Variance"] = &SourceEmitter->ZSpeedVariance;
		Tracks["Lifetime"] = &SourceEmitter->Lifetime;
		Tracks["Lifetime Variance"] = &SourceEmitter->LifetimeVariance;
		Tracks["Size Range"] = &SourceEmitter->SizeRange;
		Tracks["Size Variance"] = &SourceEmitter->SizeVariance;
		Tracks["Size"] = &SourceEmitter->Size;
		Tracks["Color"] = &SourceEmitter->Color;
		Tracks["Rotation Max"] = &SourceEmitter->MaxRotation;
		Tracks["Gravity"] = &SourceEmitter->Gravity;
		Tracks["Wind Factor"] = &SourceEmitter->WindFactor;
	}
};

class Editor
{
	std::filesystem::path ApplicationDir;
	SDL_Window* window;
	SDL_GLContext glcontext;
	Scene* ParticleScene;
public:
	Editor(int in_argc, char** in_argv);
	~Editor();

	void Draw();

	std::vector<EditEmitter> EditEmitters;

	int32_t selectedEmitter = -1;
	std::string selectedTrack;

	//Widgets
	void DrawOutliner();
};

