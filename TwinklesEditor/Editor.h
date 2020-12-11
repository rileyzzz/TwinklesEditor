#pragma once
#include <map>
#include <filesystem>
#include <numeric>
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
	std::vector<KeyframeTrackBase*> Tracks;

	EditEmitter(Emitter* InSourceEmitter) : ID(ObjectID++), SourceEmitter(InSourceEmitter)
	{
		Tracks.push_back(&SourceEmitter->EmitterSize);
		Tracks.push_back(&SourceEmitter->EmissionRate);
		Tracks.push_back(&SourceEmitter->VelocityCone);
		Tracks.push_back(&SourceEmitter->ZSpeedVariance);
		Tracks.push_back(&SourceEmitter->Lifetime);
		Tracks.push_back(&SourceEmitter->LifetimeVariance);
		Tracks.push_back(&SourceEmitter->SizeRange);
		Tracks.push_back(&SourceEmitter->SizeVariance);
		Tracks.push_back(&SourceEmitter->Size);
		Tracks.push_back(&SourceEmitter->Color);
		Tracks.push_back(&SourceEmitter->MaxRotation);
		Tracks.push_back(&SourceEmitter->Gravity);
		Tracks.push_back(&SourceEmitter->WindFactor);
		//Tracks["Emitter Size"] = &SourceEmitter->EmitterSize;
		//Tracks["Emission Rate"] = &SourceEmitter->EmissionRate;
		//Tracks["Velocity Cone"] = &SourceEmitter->VelocityCone;
		//Tracks["Z Speed Variance"] = &SourceEmitter->ZSpeedVariance;
		//Tracks["Lifetime"] = &SourceEmitter->Lifetime;
		//Tracks["Lifetime Variance"] = &SourceEmitter->LifetimeVariance;
		//Tracks["Size Range"] = &SourceEmitter->SizeRange;
		//Tracks["Size Variance"] = &SourceEmitter->SizeVariance;
		//Tracks["Size"] = &SourceEmitter->Size;
		//Tracks["Color"] = &SourceEmitter->Color;
		//Tracks["Rotation Max"] = &SourceEmitter->MaxRotation;
		//Tracks["Gravity"] = &SourceEmitter->Gravity;
		//Tracks["Wind Factor"] = &SourceEmitter->WindFactor;
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

	ImFont* MainFont = nullptr;
	ImFont* SmallFont = nullptr;

	void Draw();
	
	std::vector<EditEmitter> EditEmitters;

	
	int32_t selectedFrame = -1;
	int32_t lastSelectedFrame = -1;
	float activeEditMax = 1.0f;
	//std::pair<float, float>* selectedFramePtr = nullptr;
	//std::pair<float, float>* selectedFloatFrame = nullptr;

	EditEmitter* selectedEmitter = nullptr;
	KeyframeTrackBase* selectedTrack = nullptr;

	//Widgets
	void DrawOutliner();
	void DrawProperties();
	void DrawGraph();
};

