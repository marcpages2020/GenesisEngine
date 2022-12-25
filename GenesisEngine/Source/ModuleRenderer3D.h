#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;
typedef int GLsizei;
typedef char GLchar;

struct Buffer
{
	GLuint handle;
	GLenum type;
	uint32 size;
	uint32 head;
	void* data;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(GnEngine* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool InitOpenGL();
	update_status PreUpdate(float deltaTime);
	update_status PostUpdate(float deltaTime);
	bool CleanUp();

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private: 
	void GenerateColorTexture(GLuint& colorAttachmentHandle, vec2 displaySize, GLint internalFormat);
	void GenerateDepthTexture(GLuint& newDepthAttachmentHandle, vec2 displaySize);
	Buffer CreateBuffer(uint32 size, GLenum type, GLenum usage);

private:
	Buffer cBuffer;
	uint32 bufferHandle;
	GLint uniformBufferAlignment = -1;
	GLuint framebufferHandle = -1;

	GLuint albedoAttachmentHandle = -1;
	GLuint normalsAttachmentHandle = -1;
	GLuint positionAttachmentHandle = -1;
	GLuint depthAttachmentHandle = -1;
	GLuint metallicAttachmentHandle = -1;
	GLuint roughnessAttachmentHandle = -1;
	GLuint finalRenderAttachmentHandle = -1;
};