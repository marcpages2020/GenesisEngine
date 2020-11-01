#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"

#include <vector>
#include "FileSystem.h"

#include "parson/parson.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil/include/IL/il.h"

#pragma comment (lib, "glu32.lib")          /* link OpenGL Utility lib */
#pragma comment (lib, "opengl32.lib")     /* link Microsoft OpenGL lib */
#pragma comment (lib, "glew/libx86/glew32.lib")		  /* link glew lib */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	name = "renderer";
	context = nullptr;

	display_mode = SOLID;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG_ERROR("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum error = glewInit();

	if (error != GL_NO_ERROR)
	{
		LOG_ERROR("Error initializing glew library! %s", SDL_GetError());
		ret = false;
	}
	else
	{
		LOG("Using Glew %d.%d.%d", GLEW_VERSION_MAJOR, GLEW_VERSION_MINOR, GLEW_VERSION_MICRO);
	}

	if (ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
			LOG_ERROR("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_LIGHTING);
		lights[0].Active(true);
	}

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Projection matrix for
	int width, height;
	App->window->GetSize(width, height);
	OnResize(width, height);

	//Generate buffers

	//FrameBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//GnTexture Buffer
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, App->window->width, App->window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//RenderBuffer
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG_ERROR("Famebuffer is not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return ret;
}

bool ModuleRenderer3D::LoadConfig(JSON_Object* config)
{
	debug = json_object_get_string(config, "debug");
	vsync = json_object_get_boolean(config, "vsync");

	draw_vertex_normals = json_object_get_boolean(config, "draw_vertex_normals");
	draw_face_normals = json_object_get_boolean(config, "draw_face_normals");

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	Color c = App->camera->background;
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	DrawMeshes();
	//DrawDirectModeCube();

	return ret;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->window);

	if (App->scene->selectedGameObject != nullptr && App->scene->selectedGameObject->to_delete)
	{
		App->scene->DeleteGameObject(App->scene->selectedGameObject);
		App->scene->selectedGameObject = nullptr;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	meshes.clear();
	mesh_collections.clear();

	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &texColorBuffer);

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::AddMesh(GnMesh* mesh)
{
	meshes.push_back(mesh);
}

void ModuleRenderer3D::AddMeshCollection(GnMeshCollection* mesh)
{
	mesh_collections.push_back(mesh);
}

void ModuleRenderer3D::DrawMeshes()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->Render();
	}

	for (size_t i = 0; i < mesh_collections.size(); i++)
	{
		mesh_collections[i]->Render();
	}
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

DisplayMode ModuleRenderer3D::GetDisplayMode() { return display_mode; }

void ModuleRenderer3D::SetDisplayMode(DisplayMode display) 
{ 
	GLenum face = GL_FRONT;

	display_mode = display;

	if (!glIsEnabled(GL_CULL_FACE_MODE))
		face = GL_FRONT_AND_BACK;

	if (display == SOLID) 
	{
		glPolygonMode(face, GL_FILL);
	}
	else if (display == WIREFRAME)
	{
		glPolygonMode(face, GL_LINE);
	}
}

void ModuleRenderer3D::SetCapActive(GLenum cap, bool active)
{
	if (active)
		glEnable(cap);
	else
		glDisable(cap);
}

void ModuleRenderer3D::SetVSYNC(bool enabled)
{
	if (enabled)
	{
		if (SDL_GL_SetSwapInterval(1) == -1)
		{
			LOG_ERROR("VSYNC not supported");
		}
		else
		{
			LOG("VSYNC enabled");
		}
	}
	else
	{
		if (SDL_GL_SetSwapInterval(0) == -1)
		{
			LOG_ERROR("VSYNC not supported");
		}
		else
		{
			LOG("VSYNC disabled");
		}
	}

}

GLubyte ModuleRenderer3D::GetCheckersImage()
{
	int CHECKERS_WIDTH = 64;
	int CHECKERS_HEIGHT = 64;

	GLubyte checkerImage[64][64][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	return checkerImage[64][64][4];
}

void ModuleRenderer3D::DrawDirectModeCube()
{
	int CHECKERS_WIDTH = 64;
	int CHECKERS_HEIGHT = 64;
	GLubyte checkerImage[64][64][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	GnTexture* Lenna = TextureImporter::LoadTexture("Assets/Textures/Lenna.png");

	GLuint textureID;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Lenna->width, Lenna->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Lenna->data);

	{
		glBegin(GL_TRIANGLES);
		//Bottom Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Front Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);

		//Left Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 0.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Right Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.f, 0.f, 0.f);

		//Back Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 0.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 0.f, 0.f);

		//Top Face
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(0.f, 1.f, 1.f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.f, 1.f, 1.f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0.f, 1.f, 0.f);
		glEnd();
	}

	glDeleteTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, 0);
	ilBindImage(0);
	ilDeleteImages(1, &Lenna->id);
}

void ModuleRenderer3D::BeginDebugDraw() {}

void ModuleRenderer3D::EndDebugDraw() {}
