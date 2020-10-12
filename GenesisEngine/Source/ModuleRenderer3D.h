#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

typedef unsigned int GLuint;
typedef unsigned int GLenum;

enum DisplayMode
{
	SOLID,
	WIREFRAME
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(JSON_Object* object);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	DisplayMode GetDisplayMode();
	void SetDisplayMode(DisplayMode display);

	void SetCapActive(GLenum cap, bool active);

private:
	void BeginDebugDraw();
	void EndDebugDraw();

	GLuint frameBuffer;

public:
	GLuint texColorBuffer;
	GLuint renderBuffer;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	DisplayMode display_mode;

private:
	bool debug;
};