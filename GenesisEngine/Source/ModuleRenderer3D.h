#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "Light.h"
#include <map>

#define MAX_LIGHTS 8

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned char GLubyte;

typedef void* SDL_GLContext;

class Camera;
class GnMesh;
class ResourceShader;

enum DisplayMode
{
	SOLID,
	WIREFRAME
};

struct VertexV3V2
{
	vec3 pos;
	vec2 uv;
};

struct Quad
{
	GLuint vao;

	GLuint embeddedVertices;
	GLuint embeddedElements;

	VertexV3V2 vertices[4] = { vec3(-1.0f, -1.0f, 0.0f),  vec2(0.0f, 0.0f),
							   vec3(1.0f, -1.0f, 0.0f),  vec2(1.0f, 0.0f),
							   vec3(1.0f,  1.0f, 0.0f),  vec2(1.0f, 1.0f),
							   vec3(-1.0f,  1.0f, 0.0f),  vec2(0.0f, 1.0f)
	};

	u16 indices[6] = { 0, 1, 2, 0, 2, 3 };
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool LoadConfig(GnJSONObj& config) override;
	update_status PreUpdate(float deltaTime) override;
	update_status Update(float deltaTime) override;
	update_status PostUpdate(float deltaTime) override;
	bool CleanUp();
	
	void OnResize(int width, int height);
	void ResizeTexture(GLuint framebuffer, GLuint textureAttachmentHandle, vec2 newSize, GLuint internalFormat);

	void DrawAABB(float3* aabb);
	DisplayMode GetDisplayMode();
	void SetDisplayMode(DisplayMode display);
	void SetMainCamera(Camera* camera);
	Camera* GetMainCamera();
	bool IsInsideCameraView(AABB aabb);
	void AddBlendedMesh(float3 position, GnMesh* mesh);
	void AddMeshToRender(GnMesh* mesh);
	ResourceShader* GetDefaultShader();

	void SetCapActive(GLenum cap, bool active);
	void SetVSYNC(bool enabled);

	void DrawRay();

	bool RenderSceneGeometry();
	void GenerateQuad();
	void RenderQuad();

	bool CheckFramebufferStatus();

private:
	void GenerateBuffers();
	void GenerateColorTexture(GLuint& textureAttachmentHandle, vec2 displaySize, GLint internalFormat, GLuint glColorAttachment);
	void GenerateDepthTexture(GLuint& newDepthAttachmentHandle, vec2 displaySize);
	void BeginDebugDraw();
	void EndDebugDraw();
	void TakeScreenshot(int sceneFramebuffer);
	
	GLuint sceneFramebuffer;
	GLuint quadFramebuffer;

public:
	GLuint albedoAttachmentHandle;
	GLuint normalsAttachmentHandle;
	GLuint positionAttachmentHandle;
	GLuint finalRenderAttachmentHandle;
	GLuint depthAttachmentHandle;

	GLuint renderBuffer;
	GLuint depthRenderBuffer;

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	//mat3x3 NormalMatrix;
	//mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	DisplayMode display_mode;

	LineSegment _ray;
	bool cull_editor_camera;
	Quad quad;

	bool draw_mouse_picking_ray;
	bool draw_aabbs;
	bool draw_vertex_normals;
	bool draw_face_normals;

	bool vsync;
	ResourceShader* defaultShader;

	unsigned int quadVAO = 0;
	unsigned int quadVBO;

private: 
	Camera* _mainCamera;
	std::map<float, GnMesh*> sceneMeshes;
	std::vector<GnMesh*> meshesToRender;
	ResourceShader* quadShader;
};