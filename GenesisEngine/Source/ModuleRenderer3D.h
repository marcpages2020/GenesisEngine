#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned char GLubyte;
typedef void* SDL_GLContext;
class GnMesh;
class GnMeshCollection;

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

	bool Init();
	bool LoadConfig(JSON_Object* config) override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();
	
	void AddMesh(GnMesh* mesh);
	void AddMeshCollection(GnMeshCollection* mesh);
	void DrawMeshes();
	void OnResize(int width, int height);

	DisplayMode GetDisplayMode();
	void SetDisplayMode(DisplayMode display);

	void SetCapActive(GLenum cap, bool active);
	void SetVSYNC(bool enabled);

	GLubyte GetCheckersImage();

private:
	void DrawDirectModeCube();
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

	std::vector<GnMesh*> meshes;
	std::vector<GnMeshCollection*> mesh_collections;

	bool draw_vertex_normals;
	bool draw_face_normals;

	bool vsync;

private:
	bool debug;
};