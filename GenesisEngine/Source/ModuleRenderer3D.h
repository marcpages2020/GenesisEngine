#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Light.h"

#define MAX_LIGHTS 8

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned char GLubyte;
typedef void* SDL_GLContext;
class Camera;

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
	bool LoadConfig(GnJSONObj& config) override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();
	
	void OnResize(int width, int height);
	void UpdateProjectionMatrix(float* projectionMatrix);

	void DrawAABB(float3* aabb);
	DisplayMode GetDisplayMode();
	void SetDisplayMode(DisplayMode display);
	void SetMainCamera(Camera* camera);
	Camera* GetMainCamera();
	bool IsInsideCameraView(AABB aabb);

	void SetCapActive(GLenum cap, bool active);
	void SetVSYNC(bool enabled);

	void DrawRay();

private:
	void GenerateBuffers();
	void DrawDirectModeCube();
	void BeginDebugDraw();
	void EndDebugDraw();

	GLuint frameBuffer;

public:
	GLuint colorTexture;
	GLuint renderBuffer;

	GLuint depthRenderBuffer;
	GLuint depthTexture;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	//mat3x3 NormalMatrix;
	//mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	DisplayMode display_mode;

	LineSegment _ray;
	bool cull_editor_camera;

	bool draw_mouse_picking_ray;
	bool draw_aabbs;
	bool draw_vertex_normals;
	bool draw_face_normals;

	bool vsync;

private: 
	Camera* _mainCamera;
};