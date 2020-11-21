#include "Scene.h"

bool keyboard_w = false;
bool keyboard_a = false;
bool keyboard_s = false;
bool keyboard_d = false;
bool keyboard_q = false;
bool keyboard_e = false;

TwinklesSystem* ActiveSystem = nullptr;

void Scene::InitGL()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
	//depth/stencil renderbuffer
	
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);


	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);

	//glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glm::vec3 camPos(10.0f, 10.0f, 10.0f);
	//glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
	//cameraView = glm::lookAt(camPos,
	//						 camTarget,
	//						 glm::vec3(0.0f, 0.0f, 1.0f)); //up vector
	cameraPos = glm::vec3(10.0f, 10.0f, 10.0f);
}

Scene::~Scene()
{
	glDeleteFramebuffers(1, &fbo);
	delete ActiveSystem;
}

void Scene::Keyboard(SDL_KeyboardEvent keyevent, bool state)
{
	switch (keyevent.keysym.sym)
	{
	case SDLK_UP:
	case SDLK_w: keyboard_w = state; break;
	case SDLK_LEFT:
	case SDLK_a: keyboard_a = state; break;
	case SDLK_DOWN:
	case SDLK_s: keyboard_s = state; break;
	case SDLK_RIGHT:
	case SDLK_d: keyboard_d = state; break;
	case SDLK_q: keyboard_q = state; break;
	case SDLK_e: keyboard_e = state; break;
	}
}

void Scene::MouseWheel(SDL_MouseWheelEvent wheelevent)
{
	double target = zoom * (1.0 + (wheelevent.y * 0.25));
	if (target > 0.2) zoom = target;
}

void Scene::MouseMove(SDL_MouseMotionEvent motionevent)
{
	if (CaptureMouse)
	{
		//Rotate = false;
		//xrot += motionevent.yrel / 4.0;
		//zrot += motionevent.xrel / 4.0;
		//glm::quat xQuat = glm::quat(glm::vec3(0.0f, 0.0f, motionevent.xrel / 40.0f));
		//glm::quat yQuat = glm::quat(glm::vec3(motionevent.yrel / 40.0f, 0.0f, 0.0f));
		//glm::mat4 xMatrix(xQuat);
		//glm::mat4 yMatrix(yQuat);
		//cameraView = cameraView * xMatrix;
		//cameraView = cameraView * yMatrix;
		cameraRot += glm::vec3(motionevent.yrel / 4.0f, 0.0f, motionevent.xrel / 4.0f);
	}
}

void Scene::Mouse(SDL_MouseButtonEvent buttonevent, bool state)
{
	auto button = buttonevent.button;

	if (button == SDL_BUTTON_LEFT)
	{
		CaptureMouse = state;
		if (CaptureMouse)
			SDL_SetRelativeMouseMode(SDL_TRUE);
		else
			SDL_SetRelativeMouseMode(SDL_FALSE);
	}
}

//void Scene::ProcessEvent(const SDL_Event* event)
//{
//	switch (event->type)
//	{
//	case SDL_KEYUP:
//	case SDL_KEYDOWN:
//		Keyboard(event->key, event->type == SDL_KEYDOWN);
//		break;
//	case SDL_MOUSEWHEEL:
//		MouseWheel(event->wheel);
//		break;
//	case SDL_MOUSEBUTTONUP:
//	case SDL_MOUSEBUTTONDOWN:
//		Mouse(event->button, event->type == SDL_MOUSEBUTTONDOWN);
//		break;
//	case SDL_MOUSEMOTION:
//		MouseMove(event->motion);
//		break;
//	}
//}

void DrawGrid()
{
	int GridSize = 20;
	double GridScale = 1.0;
	glPushMatrix();
	glLineWidth(0.1);

	for (int i = 0; i <= GridSize; i++)
	{
		int gridabsolute = i - (GridSize / 2);

		glLineWidth(0.1);
		if (gridabsolute == 0) glLineWidth(0.8);

		glBegin(GL_LINES);
		glColor3f(0.4, 0.4, 0.4);
		if (gridabsolute == 0) glColor3f(0.8, 0.0, 0.0);
		glVertex3f(-(GridSize / 2) * GridScale, gridabsolute * GridScale, 0.0);
		glVertex3f((GridSize / 2) * GridScale, gridabsolute * GridScale, 0.0);
		if (gridabsolute == 0) glColor3f(0.0, 0.8, 0.0);
		glVertex3f(gridabsolute * GridScale, -(GridSize / 2) * GridScale, 0.0);
		glVertex3f(gridabsolute * GridScale, (GridSize / 2) * GridScale, 0.0);
		glEnd();
	}

	glPopMatrix();
}

void Scene::ResizeScene(int inWidth, int inHeight)
{
	std::cout << "window resize " << inWidth << " " << inHeight << "\n";
	width = inWidth;
	height = inHeight;

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

uint32_t Scene::FirstUnusedParticle()
{
	static uint32_t lastUsed = 0;

	for (uint32_t i = lastUsed; i < particleCount; i++)
	{
		if (particles[i].Life <= 0.0f)
		{
			lastUsed = i;
			return i;
		}
	}

	//linear search
	for (uint32_t i = 0; i < lastUsed; i++)
	{
		if (particles[i].Life <= 0.0f)
		{
			lastUsed = i;
			return i;
		}
	}

	lastUsed = 0;
	return 0;
}

#define randfloat() ((float)std::rand() / (float)RAND_MAX - 0.5f) * 2.0f
void Scene::ParticleRespawn(RenderParticle& particle)
{
	//float random = (float)std::rand() / (float)RAND_MAX;


	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = glm::vec3(randfloat() * 1.0f, randfloat() * 1.0f, randfloat() * 2.0f);
}

void Scene::Render(float deltaTime)
{
	//Spawn new particles
	uint32_t rate = 2;
	for (uint32_t i = 0; i < rate; i++)
	{
		uint32_t FirstUnused = FirstUnusedParticle();
		ParticleRespawn(particles[FirstUnused]);
	}

	//Update particles
	for (auto& particle : particles)
	{
		particle.Life -= deltaTime;
		if (particle.Life > 0.0f)
		{
			particle.Position += particle.Velocity * deltaTime;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(5 / (double)255, 5 / (double)255, 5 / (double)255, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glLoadIdentity();

	//glPushMatrix();
	//glTranslatef(0.0f, 0.0f, -40.0f);
	////glTranslatef(0.0f, 0.0f, -40.0f);
	////glMultMatrixf(glm::value_ptr(cameraView));

	//DrawGrid();
	//glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	glTranslatef(0.0f, 0.0f, -40.0f);
	//glMultMatrixf(glm::value_ptr(cameraView));
	//glm::mat4 cameraMatrix = glm::mat4(glm::quat(cameraRot));
	//glMultMatrixf(glm::value_ptr(cameraMatrix));

	glRotatef(cameraRot.x, 1.0f, 0.0f, 0.0f);
	glRotatef(cameraRot.y, 0.0f, 1.0f, 0.0f);
	glRotatef(cameraRot.z, 0.0f, 0.0f, 1.0f);

	glScalef(zoom, zoom, zoom);

	DrawGrid();

	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_POINTS);
	for (const auto& particle : particles)
	{
		glVertex3fv(glm::value_ptr(particle.Position));
	}
	glEnd();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::OpenFile(const char* path)
{
	delete ActiveSystem;
	ActiveSystem = new TwinklesSystem(path);
}

void Scene::ExportFile(const char* path)
{

}
