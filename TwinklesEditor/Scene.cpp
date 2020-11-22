#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool keyboard_w = false;
bool keyboard_a = false;
bool keyboard_s = false;
bool keyboard_d = false;
bool keyboard_q = false;
bool keyboard_e = false;

//TwinklesSystem* ActiveSystem = nullptr;

Scene::Scene(std::string InDirectory) : Directory(InDirectory)
{
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -40.0f));
	view = glm::rotate(view, glm::radians(-75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::rotate(view, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	projection = glm::perspective(glm::radians(45.0f), 1024.0f / 1024.0f, 0.1f, 1000.0f);

	particleShader = new Shader((Directory + "/shaders/pfx.vert").c_str(), (Directory + "/shaders/pfx.frag").c_str(), (Directory + "/shaders/pfx.geom").c_str());
	//particleShader = new Shader((Directory + "/shaders/pfx.vert").c_str(), (Directory + "/shaders/pfx.frag").c_str());
}

Scene::~Scene()
{
	glDeleteFramebuffers(1, &fbo);
	delete particleShader;

	//delete ActiveSystem;
}

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
	//glEnable(GL_CULL_FACE);

	//glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//glm::vec3 camPos(10.0f, 10.0f, 10.0f);
	//glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
	//cameraView = glm::lookAt(camPos,
	//						 camTarget,
	//						 glm::vec3(0.0f, 0.0f, 1.0f)); //up vector
	cameraPos = glm::vec3(10.0f, 10.0f, 10.0f);
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
	if (target > 0.2)
	{
		zoom = target;
		view = glm::scale(view, glm::vec3(1.0 + (wheelevent.y * 0.25)));
	}
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
		//cameraRot += glm::vec3(motionevent.yrel / 4.0f, 0.0f, motionevent.xrel / 4.0f);
		
		const glm::mat4 inverted = glm::inverse(view);
		const glm::vec3 right = normalize(glm::vec3(inverted[0]));

		//view = glm::rotate(view, motionevent.yrel / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, motionevent.yrel / 100.0f, right);
		view = glm::rotate(view, motionevent.xrel / 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		//cameraRot += glm::vec3(motionevent.yrel / 4.0f, 0.0f, motionevent.xrel / 4.0f);
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

	projection = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderEmitter::RenderEmitter(Emitter& emit, Scene* InScene) : SourceEmitter(emit), parentScene(InScene)
{
	particleShader = parentScene->particleShader;

	KUID& TexKUID = SourceEmitter.TextureKUID;
	if (TexKUID.UserID == -1 && TexKUID.ContentID == 6329)
		SetTexture((parentScene->Directory + "/assets/steam.tga").c_str());
	else if (TexKUID.UserID == -1 && TexKUID.ContentID == 6330)
		SetTexture((parentScene->Directory + "/assets/smoke.tga").c_str());
	else if (TexKUID.UserID == -1 && TexKUID.ContentID == 6339)
		SetTexture((parentScene->Directory + "/assets/spark.tga").c_str());
	else
		SetTexture((parentScene->Directory + "/assets/unknown.tga").c_str());

	for (unsigned int i = 0; i < particleCount; ++i)
		particles.push_back(RenderParticle());

	glGenBuffers(1, &EmitterVBO);
	glGenVertexArrays(1, &EmitterVAO);

	glBindVertexArray(EmitterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, EmitterVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderParticle) * particleCount, &particles[0], GL_DYNAMIC_DRAW);


	//attributes bound to current VBO

	//position
	glEnableVertexAttribArray(0); //corresponds to layout location
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)0);
	//glVertexAttribDivisor(0, 1);

	//velocity
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Velocity));
	//glVertexAttribDivisor(1, 1);

	//color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Color));
	//glVertexAttribDivisor(2, 1);

	//size
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Size));

	//rotation
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Rotation));

	//life
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Life));
	//glVertexAttribDivisor(3, 1);

	//random
	//glEnableVertexAttribArray(5);
	//glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Rand));

	////transform
	//constexpr std::size_t vec4size = sizeof(glm::vec4);
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)offsetof(RenderParticle, Transform));
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)(offsetof(RenderParticle, Transform) + 1 * vec4size));
	//glEnableVertexAttribArray(5);
	//glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)(offsetof(RenderParticle, Transform) + 2 * vec4size));
	//glEnableVertexAttribArray(6);
	//glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticle), (void*)(offsetof(RenderParticle, Transform) + 3 * vec4size));
	//
	////instancing
	//glVertexAttribDivisor(3, 1);
	//glVertexAttribDivisor(4, 1);
	//glVertexAttribDivisor(5, 1);
	//glVertexAttribDivisor(6, 1);



	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//std::cout << "Color along:\n";
	//for (float i = 0.0f; i < 1.0f; i += 0.1f)
	//{
	//	Color color = SourceEmitter.Color.GetKey(i);
	//	std::cout << color << "\n";
	//	//std::cout << "color " << particles[0].Color.r << " " << particles[0].Color.g << " " << particles[0].Color.b << " " << particles[0].Color.a << "\n";
	//}
}

uint32_t RenderEmitter::FirstUnusedParticle()
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
void RenderEmitter::ParticleRespawn(RenderParticle& particle)
{
	//float random = (float)std::rand() / (float)RAND_MAX;
	particle.Rand = (float)std::rand() / (float)RAND_MAX;

	particle.Position = SourceEmitter.Position.ToGLM();

	glm::vec3 EmitterSize = SourceEmitter.EmitterSize.GetKey(0.0f).ToGLM();
	particle.Position.x += EmitterSize.x * randfloat();
	particle.Position.y += EmitterSize.y * randfloat();
	particle.Position.z += EmitterSize.z * randfloat();


	//particle.Transform = glm::mat4();
	//particle.Transform = glm::translate(particle.Transform, SourceEmitter.Position.ToGLM());
	particle.Color = SourceEmitter.Color.GetKey(0.0f).ToGLM();
	particle.Life = 1.0f;
	//z is fixed, x and y vary
	glm::vec3 VelocityCone = SourceEmitter.VelocityCone.GetKey(0.0f).ToGLM();
	VelocityCone.x *= randfloat();
	VelocityCone.y *= randfloat();
	VelocityCone.z += SourceEmitter.ZSpeedVariance.GetKey(0.0f) * randfloat();
	particle.Velocity = VelocityCone;

	//glm::quat testRot(glm::vec3(-45.0f, 0.0f, 0.0f));
	//std::cout << "rotation " << SourceEmitter.Rotation.x << " " << SourceEmitter.Rotation.y << " " << SourceEmitter.Rotation.z << " " << SourceEmitter.Rotation.w << "\n";
	//std::cout << "rotation " << testRot.x << " " << testRot.y << " " << testRot.z << " " << testRot.w << "\n";

	glm::mat4 rotationMat(SourceEmitter.Rotation.ToGLM());
	particle.Velocity = glm::vec3(rotationMat * glm::vec4(particle.Velocity, 1.0f));

	particle.RotationRate = SourceEmitter.MaxRotation.GetKey(0.0f) * randfloat();

	//size_control*(max_size-min_size) + size_control*size_variance
	float size_control = SourceEmitter.Size.GetKey(0.0f);
	glm::vec2 MinMax = SourceEmitter.SizeRange.GetKey(0.0f).ToGLM();
	float min_size = MinMax.x;
	float max_size = MinMax.y;
	float size_variance = SourceEmitter.SizeVariance.GetKey(0.0f) * particle.Rand;

	if (max_size < min_size)
		max_size = min_size;
	particle.Size = size_control * (max_size - min_size) + size_control * size_variance;
	particle.Size = std::clamp(particle.Size, min_size, max_size);
}

void RenderEmitter::SetTexture(const char* path)
{
	if (TextureLoaded)
		glDeleteTextures(1, &SpriteTex);

	TextureLoaded = true;
	glGenTextures(1, &SpriteTex);
	int width, height, nrComponents;
	uint8_t* data = stbi_load(path, &width, &height, &nrComponents, 0);

	GLenum format = GL_RGBA;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, SpriteTex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);
}

void RenderEmitter::EmitterTick(float deltaTime)
{
	//Spawn new particles
	particleAccumulator += deltaTime;
	const float SecondsPerParticle = 1.0f / SourceEmitter.EmissionRate.GetKey(0.0f);
	while(particleAccumulator > SecondsPerParticle) //seconds / particle
	{
		particleAccumulator -= SecondsPerParticle;

		uint32_t FirstUnused = FirstUnusedParticle();
		ParticleRespawn(particles[FirstUnused]);
	}

	float Lifetime = SourceEmitter.Lifetime.GetKey(0.0f);
	float LifetimeVariance = SourceEmitter.LifetimeVariance.GetKey(0.0f);

	//Update particles
	for (auto& particle : particles)
	{
		particle.Life -= deltaTime / (Lifetime + LifetimeVariance * ((particle.Rand - 0.5f) * 2.0f));
		if (particle.Life > 0.0f)
		{
			particle.Velocity.z -= SourceEmitter.Gravity.GetKey(1.0f - particle.Life) / 40.0f;
			//particle.Transform = glm::translate(particle.Transform, particle.Velocity * deltaTime);
			//particle.
			particle.Position += particle.Velocity * deltaTime;
			particle.Color = SourceEmitter.Color.GetKey(1.0f - particle.Life).ToGLM();
			particle.Rotation += particle.RotationRate;

			float size_control = SourceEmitter.Size.GetKey(1.0f - particle.Life);
			//0 on purpose
			glm::vec2 MinMax = SourceEmitter.SizeRange.GetKey(0.0f).ToGLM();
			float min_size = MinMax.x;
			float max_size = MinMax.y;
			//0 on purpose
			float size_variance = SourceEmitter.SizeVariance.GetKey(0.0f) * particle.Rand; // * randfloat()
			
			if (max_size < min_size)
				max_size = min_size;
			particle.Size = size_control * (max_size - min_size) + size_control * size_variance;
			particle.Size = std::clamp(particle.Size, min_size, max_size);
		}
	}
}

void RenderEmitter::DrawParticles()
{
	//glColor3f(0.8, 0.8, 0.8);
	//glBegin(GL_POINTS);
	
	//std::cout << "color " << particles[0].Color.r << " " << particles[0].Color.g << " " << particles[0].Color.b << " " << particles[0].Color.a << "\n";

	//sort particles array for transparency
	//glm::mat4 inverse = glm::inverse(parentScene->view);
	//glm::vec3 camerapos = glm::vec3(inverse[3]); // / inverse[3].w
	//std::map<float, RenderParticle> sorted;
	//for (auto& particle : particles)
	//{
	//	float distance = glm::length(particle.Position - camerapos);
	//	sorted[distance] = particle;
	//}
	//std::vector<RenderParticle> FinalArray;
	//for (auto& particle : sorted)
	//	FinalArray.push_back(particle.second);

	glBindBuffer(GL_ARRAY_BUFFER, EmitterVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RenderParticle) * particleCount, &particles[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RenderParticle) * FinalArray.size(), &FinalArray[0]);

	//glm::mat4 modelview;
	//glm::mat4 proj;
	//glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(modelview));
	//glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(proj));


	//Additive blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	particleShader->use();

	particleShader->setMat4("view", parentScene->view);
	particleShader->setMat4("projection", parentScene->projection);
	particleShader->setInt("type", (uint32_t)SourceEmitter.Type);

	glBindTexture(GL_TEXTURE_2D, SpriteTex);
	glBindVertexArray(EmitterVAO);
	//glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);
	//glDrawArraysInstanced(GL_POINTS, 0, 1, particleCount);
	glDrawArrays(GL_POINTS, 0, particleCount);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBegin(GL_POINTS);
	//for (const auto& particle : particles)
	//{
	//	if (particle.Life > 0.0f)
	//	{
	//		glPushMatrix();
	//		//glTranslatef(particle.Position.x, );
	//		//glMultMatrixf(glm::value_ptr(particle.Transform));
	//		glVertex3fv(glm::value_ptr(particle.Position));
	//		//particleShader->setVec4("color", particle.Color);

	//		glPopMatrix();
	//	}
	//}
	//glEnd();
	glUseProgram(0);
}

void Scene::Render(float deltaTime)
{

	for (auto& emitter : Emitters)
		emitter.EmitterTick(deltaTime);

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
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
	glMultMatrixf(glm::value_ptr(projection));
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix
	glMultMatrixf(glm::value_ptr(view));

	//glTranslatef(0.0f, 0.0f, -40.0f);
	//glMultMatrixf(glm::value_ptr(cameraView));
	//glm::mat4 cameraMatrix = glm::mat4(glm::quat(cameraRot));
	//glMultMatrixf(glm::value_ptr(cameraMatrix));

	//glRotatef(cameraRot.x, 1.0f, 0.0f, 0.0f);
	//glRotatef(cameraRot.y, 0.0f, 1.0f, 0.0f);
	//glRotatef(cameraRot.z, 0.0f, 0.0f, 1.0f);

	//glScalef(zoom, zoom, zoom);

	DrawGrid();

	glDepthMask(GL_FALSE);
	for (auto& emitter : Emitters)
		emitter.DrawParticles();
	glDepthMask(GL_TRUE);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::OpenFile(const char* path)
{
	//delete ActiveSystem;
	//ActiveSystem = new TwinklesSystem(path);
	ActiveSystem = TwinklesSystem(path);

	for(auto& emitter : ActiveSystem.Emitters)
		Emitters.emplace_back(emitter, this);
}

void Scene::ExportFile(const char* path)
{

}
