#pragma once
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
};

