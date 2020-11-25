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


	int scrw = 1400;
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
	MainFont = io.Fonts->AddFontFromFileTTF((ApplicationDir.string() + "/Roboto-Regular.ttf").c_str(), 18.0f, &config);
	SmallFont = io.Fonts->AddFontFromFileTTF((ApplicationDir.string() + "/Roboto-Regular.ttf").c_str(), 12.0f, &config);


	ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
	ImGui_ImplOpenGL3_Init();

	ParticleScene = new Scene(ApplicationDir.string());

	ParticleScene->InitGL();
	ParticleScene->ResizeScene(scrw, scrh);
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

		ImGui::SetNextWindowPos(ImVec2(0, 25), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(200, scrh - 200 - 20), ImGuiCond_FirstUseEver);
		ImGui::Begin("Outliner", nullptr,
			ImGuiWindowFlags_NoSavedSettings);
		DrawOutliner();
		ImGui::End();
		
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		static int GraphHeight = 200;

		//ImVec2 windowSize = ImVec2(scrw, scrh);
		ImGui::SetNextWindowPos(ImVec2(0, h - GraphHeight), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(w, GraphHeight), ImGuiCond_Always);
		ImGui::Begin("Graph", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | //ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoSavedSettings);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
		if (ImGui::IsItemActive())
			GraphHeight -= ImGui::GetIO().MouseDelta.y;
		ImGui::Separator();
		ImGui::PopStyleVar();

		DrawGraph();
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
				ObjectID = 0;
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
					auto& emitters = ParticleScene->ActiveSystem.Emitters;
					selectedEmitter = -1;
					selectedTrack = nullptr;
					EditEmitters.clear();
					for (auto& emit : emitters)
						EditEmitters.emplace_back(&emit);
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

	ImGui::Dummy(ImVec2(0, 16));

	//float w = ImGui::GetWindowContentRegionMax().x;
	//static float h = ImGui::GetWindowContentRegionMax().y - 200;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::BeginChild("Scene"); //, ImVec2(w, h), true, ImGuiWindowFlags_NoScrollbar
	//ImVec2 wsize = ImGui::GetWindowSize();
	ImVec2 wsize = ImGui::GetWindowContentRegionMax();
	//ImVec2 wsize = ImVec2(w, h);
	ImGui::Image((ImTextureID)ParticleScene->texColorBuffer, wsize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::PopStyleVar();

	//ImGui::InvisibleButton("hsplitter", ImVec2(-1, 2.0f));
	//if (ImGui::IsItemActive())
	//{
	//	h += ImGui::GetIO().MouseDelta.y;

	//}

	//ImGui::BeginChild("Graph", ImVec2(0, 0), true);

	//ImGui::EndChild();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsItemClicked())
	{
		ParticleScene->CaptureMouse = true;
		io.WantCaptureMouse = true;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

void Editor::DrawOutliner()
{
	ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	//auto& emitters = ParticleScene->ActiveSystem.Emitters;

	for (const auto& emit : EditEmitters)
	{
		Emitter* SourceEmitter = emit.SourceEmitter;
		std::string uniqueName = "Emitter " + std::to_string(emit.ID);
		if (ImGui::TreeNode(uniqueName.c_str()))
		{
			for (uint32_t i = 0; i < emit.Tracks.size(); i++)
			{
				KeyframeTrackBase* track = emit.Tracks[i];
				ImGuiTreeNodeFlags flags = TreeFlags;
				if (track == selectedTrack)
					flags |= ImGuiTreeNodeFlags_Selected;

				std::string itemname = track->name + "##" + std::to_string(emit.ID);
				ImGui::TreeNodeEx(itemname.c_str(), flags);

				if (ImGui::IsItemClicked())
				{
					selectedEmitter = emit.ID;
					selectedTrack = track;
				}
			}
			ImGui::TreePop();
		}
	}
}

void DrawEditCursor(ImVec2 pos, bool selected)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	constexpr float size = 8.0f;
	draw_list->AddCircle(pos, size, 0xFF808080, 12);
	draw_list->AddCircle(pos, size + 1, selected ? 0xFF14FF14 : 0xFFA8A8A8, 12);
}

void DrawGraphLine(ImVec2 A, ImVec2 B)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	draw_list->AddLine(A, B, 0xFF1B1B9E, 1.5f);
}

//template <class T>
//void DrawTrack(KeyframeTrack<T>* Track)
//{
//
//}

inline bool InsideContainer(ImVec2 test, ImVec2 a, ImVec2 b)
{
	return test.x > a.x && test.y > a.y && test.x < b.x && test.y < b.y;
}

void Editor::DrawGraph()
{
	if (selectedTrack)
	{
		ImGui::Text(selectedTrack->name.c_str());
		EditEmitter& emit = EditEmitters[selectedEmitter];
		
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 0.25f));
		ImGui::BeginChild("Graph", ImVec2(-64.0f, 0.0f), true);
		ImVec2 size = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		windowPos.x += contentMin.x / 2;
		windowPos.y += contentMin.y / 2;

		ImVec2 screenContentMax = windowPos;
		screenContentMax.x += size.x;
		screenContentMax.y += size.y;

		//ImVec2 windowPos = ImGui::GetWindowContentRegionMin();

		//std::map<KeyframeTrack<float>*, float>
		//std::cout << "pos " << windowPos.x << " " << windowPos.y << "\n";
		//std::cout << "size " << size.x << " " << size.y << "\n";

		float maxHeight = 1.0f;
		if (selectedTrack->type == typeid(float))
		{
			auto* Track = dynamic_cast<KeyframeTrack<float>*>(selectedTrack);
			if (selectedFrame == -1)
			{
				for (auto& frame : Track->Frames)
					maxHeight = std::max(maxHeight, frame.second);
			}
			else
				maxHeight = activeEditMax;
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImU32 lineColor(0xFF545454);
		for (float i = 0.0f; i < size.x; i += size.x / 20.0f)
			draw_list->AddLine(ImVec2(windowPos.x + i, windowPos.y), ImVec2(windowPos.x + i, windowPos.y + size.y), lineColor);

		/*for (float i = 0.0f; i < size.y; i += size.y / 4.0f)
			draw_list->AddLine(ImVec2(windowPos.x, windowPos.y + i), ImVec2(windowPos.x + size.x, windowPos.y + i), lineColor);*/

		constexpr float unitSpacing = 10.0f;
		for (float i = 0.0f; i <= maxHeight; i += maxHeight / unitSpacing)
		{
			float y = screenContentMax.y - (i / maxHeight) * size.y;
			draw_list->AddLine(ImVec2(windowPos.x, y), ImVec2(windowPos.x + size.x, y), lineColor);
		}

		

		if (selectedTrack->type == typeid(float))
		{
			auto* Track = dynamic_cast<KeyframeTrack<float>*>(selectedTrack);
			auto& Frames = Track->Frames;


			ImVec2 mousePos = ImGui::GetMousePos();

			std::vector<size_t> sortedMap(Frames.size());
			std::iota(sortedMap.begin(), sortedMap.end(), 0);
			std::sort(sortedMap.begin(), sortedMap.end(),
				[Frames](size_t a, size_t b) { return Frames[a].first < Frames[b].first; });

			//static std::pair<float, float>* selectedFramePtr = nullptr;

			for (uint32_t i = 0; i < Frames.size(); i++)
			{
				//sorted
				//auto& frame = Frames[sortedMap[i]];
				auto& frame = Frames[i];

				float x = frame.first * size.x;
				float y = (1.0f - frame.second / maxHeight) * size.y;
				ImVec2 pos((float)windowPos.x + x, (float)windowPos.y + y);

				auto next = Track->GetNextFrame(frame.first);
				if (next.first != frame.first)
				{
					float nextx = next.first * size.x;
					float nexty = (1.0f - next.second / maxHeight) * size.y;
					ImVec2 nextPos((float)windowPos.x + nextx, (float)windowPos.y + nexty);
					DrawGraphLine(pos, nextPos);
				}
				
				DrawEditCursor(pos, selectedFrame == sortedMap[i]);
				if (selectedFrame == sortedMap[i])
				{
					auto& frame = Frames[selectedFrame];
					ImGui::BeginTooltip();
					std::stringstream first;
					first << std::setprecision(4) << std::noshowpoint << frame.first;
					first << ", ";
					first << std::setprecision(4) << std::noshowpoint << frame.second;
					ImGui::Text(first.str().c_str());
					ImGui::EndTooltip();
				}

				if (selectedFrame == -1 && ImGui::IsItemActive()) //selectedFrame == -1 
				{
					float dist = std::hypotf(pos.x - mousePos.x, pos.y - mousePos.y);

					if (dist < 15.0f)
					{ 
						selectedFrame = sortedMap[i];
						activeEditMax = maxHeight;
						//selectedFramePtr = &frame;
						//std::cout << "now selected " << selectedFrame << "\n";
					}
				}
			}

			bool MouseInside = InsideContainer(mousePos, windowPos, screenContentMax);
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && MouseInside)
			{
				float first = (mousePos.x - windowPos.x) / size.x;
				float second = (1.0f - ((mousePos.y - windowPos.y) / size.y)) * maxHeight;
				selectedFrame = Frames.size();
				activeEditMax = maxHeight;
				Frames.emplace_back(first, second);
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && selectedFrame == -1 && MouseInside)
			{
				for (auto& frame : Frames)
				{
					if (frame.first != 0.0f && frame.first != 1.0f)
					{
						float x = frame.first * size.x;
						float y = (1.0f - frame.second / maxHeight) * size.y;
						ImVec2 pos(windowPos.x + x, windowPos.y + y);

						float dist = std::hypotf(pos.x - mousePos.x, pos.y - mousePos.y);
						if (dist < 15.0f)
						{
							std::remove(Frames.begin(), Frames.end(), frame);
							break;
						}
					}
				}
			}

			if (selectedFrame != -1)
			{
				auto& editFrame = Frames[selectedFrame];
				if (editFrame.first != 0.0f && editFrame.first != 1.0f)
				{
					editFrame.first += (ImGui::GetIO().MouseDelta.x / size.x);
					editFrame.first = std::clamp(editFrame.first, 0.001f, 0.999f);
				}
				editFrame.second -= (ImGui::GetIO().MouseDelta.y * maxHeight) / size.y;

				auto FrameCopy = Frames[selectedFrame];
				std::sort(Frames.begin(), Frames.end());
				selectedFrame = std::find(Frames.begin(), Frames.end(), FrameCopy) - Frames.begin();
			}
		}
		//ImGui::EndChild();
		//ImGui::PopStyleColor();

		//static bool pendingChanges = false;
		//if (ImGui::IsItemClicked())
		//{
		//	pendingChanges = true;

		//}
		//static bool hasTarget = false;
		//static std::pair<float, float> targetEdit = std::make_pair<float, float>(-1.0f, -1.0f);

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			selectedFrame = -1;

		ImGui::EndChild();
		ImGui::PopStyleColor();

		//draw units
		ImGui::PushFont(SmallFont);
		for (float i = 0.0f; i <= maxHeight; i += maxHeight / unitSpacing)
		{
			float y = screenContentMax.y - (i / maxHeight) * size.y;
			ImVec2 pos(screenContentMax.x + 8, y - 8);
			std::stringstream text;
			text << std::setprecision(4) << std::noshowpoint << i;
			draw_list->AddText(pos, 0xFFFFFFFF, text.str().c_str());
		}
		ImGui::PopFont();

		//static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
		//ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));
	}
}
