#include "Editor.h"

Editor::Editor(int argc, char** argv)
{
	ApplicationDir = std::filesystem::path(argv[0]).parent_path();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	//SDL_INIT_EVERYTHING
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
	}


	int scrw = 1024;
	int scrh = 1024;
	window = SDL_CreateWindow("Twinkles Editor",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		scrw, scrh, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	glcontext = SDL_GL_CreateContext(window);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to init.\n";
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
	ImGui::StyleColorsDark();

	ImFontConfig config;
	config.OversampleH = 3;
	config.OversampleV = 1;
	auto MainFont = io.Fonts->AddFontFromFileTTF((ApplicationDir.string() + "/Roboto-Regular.ttf").c_str(), 18.0f, &config);


	ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
	ImGui_ImplOpenGL3_Init();

	ParticleScene = new Scene;

	ParticleScene->InitGL();
	ParticleScene->ResizeScene(1024, 1024);
	//MyContext = new LibGLContext(wglGetCurrentDC(), wglGetCurrentContext());
	int close = 0;
	while (!close)
	{
		SDL_GL_MakeCurrent(window, glcontext);
		SDL_Event event;
		// Events mangement 
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			//ParticleScene->ProcessEvent(&event);
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				close = 1; //close all windows
				break;
			}
			if (io.WantCaptureKeyboard)
			{
				if(event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
					ParticleScene->Keyboard(event.key, event.type == SDL_KEYDOWN);
			}
			if (io.WantCaptureMouse)
			{
				switch (event.type)
				{
				case SDL_MOUSEWHEEL:
					ParticleScene->MouseWheel(event.wheel);
					break;
				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						ParticleScene->CaptureMouse = false;
						SDL_SetRelativeMouseMode(SDL_FALSE);
						io.WantCaptureMouse = false;
					}
					break;
				//case SDL_MOUSEBUTTONUP:
				//case SDL_MOUSEBUTTONDOWN:
					//ParticleScene->Mouse(event.button, event.type == SDL_MOUSEBUTTONDOWN);
					//break;
				case SDL_MOUSEMOTION:
					ParticleScene->MouseMove(event.motion);
					break;
				}
			}
			switch (event.type)
			{
			case SDL_DROPFILE:

				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					glViewport(0, 0, event.window.data1, event.window.data2);
					ParticleScene->ResizeScene(event.window.data1, event.window.data2);
				}
				break;
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		//// Frame logic here...
		
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::Begin("Content", nullptr,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::PushFont(MainFont);
		Draw();
		ImGui::PopFont();

		ImGui::End();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ParticleScene->Render(io.DeltaTime);

		SDL_GL_SwapWindow(window);


		//SDL_Delay(1000.0 / 60.0);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

Editor::~Editor()
{
	delete ParticleScene;
}

void Editor::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{

			}
			if (ImGui::MenuItem("Open"))
			{
				char szFile[255];
				OPENFILENAMEA ofn;
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Twinkles PFX (*.tfx)\0*.tfx\0All Files (*.*)\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (GetOpenFileNameA(&ofn))
				{
					//LoadMesh(szFile);
					ParticleScene->OpenFile(szFile);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	

	ImGui::BeginChild("Scene");
	ImVec2 wsize = ImGui::GetWindowSize();
	ImGui::Image((ImTextureID)ParticleScene->texColorBuffer, wsize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsItemClicked())
	{
		ParticleScene->CaptureMouse = true;
		io.WantCaptureMouse = true;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

}
