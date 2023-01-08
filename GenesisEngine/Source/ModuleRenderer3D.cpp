#include "Globals.h"
#include "Engine.h"
#include <vector>

#include "GameObject.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"

#include "FileSystem.h"
#include "ShaderImporter.h"
#include "GnJSON.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "glad/include/glad/glad.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil/include/IL/il.h"
#include "TextureImporter.h"

#include "ResourceShader.h"
#include <Devil/include/IL/ilut.h>

#pragma comment (lib, "glu32.lib")          /* link OpenGL Utility lib */
#pragma comment (lib, "opengl32.lib")     /* link Microsoft OpenGL lib */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled),
defaultShader(nullptr), quadShader(nullptr), context(nullptr), cull_editor_camera(true), _mainCamera(nullptr),
albedoAttachmentHandle(0u), normalsAttachmentHandle(0u), positionAttachmentHandle(0u), finalRenderAttachmentHandle(0u),
draw_aabbs(false), draw_mouse_picking_ray(false), draw_vertex_normals(false), draw_face_normals(false),
sceneFramebuffer(0u), quadFramebuffer(0u), renderBuffer(0), depthAttachmentHandle(0), depthRenderBuffer(0), display_mode(SOLID), vsync(false)
{
	name = "renderer";
	_ray = LineSegment();
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

bool ModuleRenderer3D::LoadConfig(GnJSONObj& config)
{
	//debug = config.GetBool("debug");
	vsync = config.GetBool("vsync");

	draw_aabbs = config.GetBool("draw_aabbs");
	draw_vertex_normals = config.GetBool("draw_vertex_normals");
	draw_face_normals = config.GetBool("draw_face_normals");

	return true;
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(engine->window->window);
	if (context == NULL) {
		LOG_ERROR("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	//Setup the context
	if (SDL_GL_MakeCurrent(engine->window->window, context) != 0)
	{
		LOG_ERROR("Failed to make OpenGL context current: %s", SDL_GetError());
		SDL_GL_DeleteContext(context);
		ret = false;
	}

	//Load Glad
	if (!gladLoadGL()) {
		LOG_ERROR("Failed to initialize OpenGL context\n");
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG_ERROR("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			//ret = false;
		}

		//Initialize clear color
		glClearColor(0.0f, 0.0f, 0.0f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG_ERROR("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_STENCIL_TEST);
		//glEnable(GL_BLEND);
	}

	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GenerateBuffers();
	GenerateQuad();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float deltaTime)
{
	sceneMeshes.clear();
	meshesToRender.clear();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float deltaTime)
{
	update_status ret = UPDATE_CONTINUE;

	return ret;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float deltaTime)
{
	//if (draw_mouse_picking_ray)
	//	DrawRay();

	//Render all scene geometry into different textures
	RenderSceneGeometry();

	//Take the previous textures, apply lighting and set it into a quad
	RenderQuad();

	//Render the editor
	RenderEditor();

	SDL_GL_SwapWindow(engine->window->window);

	if (engine->scene->selectedGameObject != nullptr && engine->scene->selectedGameObject->to_delete)
	{
		engine->scene->DeleteGameObject(engine->scene->selectedGameObject);
		engine->scene->selectedGameObject = nullptr;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	//Delete the framebuffer
	glDeleteFramebuffers(1, &sceneFramebuffer);
	glDeleteFramebuffers(1, &quadFramebuffer);
	
	//Delete all the used textures
	glDeleteTextures(1, &albedoAttachmentHandle);
	glDeleteTextures(1, &normalsAttachmentHandle);
	glDeleteTextures(1, &positionAttachmentHandle);
	glDeleteTextures(1, &finalRenderAttachmentHandle);

	//Delete the context
	SDL_GL_DeleteContext(context);
	
	_mainCamera = nullptr;

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	imageSize = vec2(width, height);

	//Scene textures
	ResizeTexture(sceneFramebuffer, albedoAttachmentHandle, imageSize, GL_RGBA8);
	ResizeTexture(sceneFramebuffer, normalsAttachmentHandle, imageSize, GL_RGBA16F);
	ResizeTexture(sceneFramebuffer, positionAttachmentHandle, imageSize, GL_RGBA16F);
	
	//Quad texture
	ResizeTexture(quadFramebuffer, finalRenderAttachmentHandle, imageSize, GL_RGBA16F);

	//glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer3D::ResizeTexture(GLuint framebuffer, GLuint textureAttachmentHandle, vec2 newSize, GLuint internalFormat)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glBindTexture(GL_TEXTURE_2D, textureAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newSize.x, newSize.y, 0, internalFormat, GL_FLOAT, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::DrawAABB(float3* cornerPoints)
{
	if (draw_aabbs == false)
		return;

	//glBegin(GL_LINES);

	/*glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);*/

	//glEnd();
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

void ModuleRenderer3D::SetMainCamera(Camera* camera)
{
	_mainCamera = camera;
}

Camera* ModuleRenderer3D::GetMainCamera()
{
	return _mainCamera;
}

bool ModuleRenderer3D::IsInsideCameraView(AABB aabb)
{
	if (cull_editor_camera)
		return _mainCamera->ContainsAABB(aabb) || engine->camera->GetCamera()->ContainsAABB(aabb);
	else
		return _mainCamera->ContainsAABB(aabb);
}

void ModuleRenderer3D::AddBlendedMesh(float3 position, GnMesh* mesh)
{
	float distance = engine->camera->GetPosition().Sub(position).Length();
	sceneMeshes[distance] = mesh;
}

void ModuleRenderer3D::AddMeshToRender(GnMesh* mesh)
{
	meshesToRender.push_back(mesh);
}

ResourceShader* ModuleRenderer3D::GetDefaultShader()
{
	ResourceShader* defaultShader = (ResourceShader*)engine->resources->RequestResource(engine->resources->Find("Assets/EngineAssets/default_shader.vert"));

	if (defaultShader == nullptr)
	{
		uint defaultShaderID = engine->resources->CreateNewResource(RESOURCE_SHADER, "default_shader", "Assets/EngineAssets/default_shader");
		defaultShader = (ResourceShader*)engine->resources->RequestResource(engine->resources->Find("Assets/EngineAssets/default_shader.vert"));
	}

	return defaultShader;
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

void ModuleRenderer3D::DrawRay()
{
	//glBegin(GL_LINES);
	//glColor3f(0.0f, 0.85f, 0.85f);
	//glVertex3f(_ray.a.x, _ray.a.y, _ray.a.z);
	//glVertex3f(_ray.b.x, _ray.b.y, _ray.b.z);
	//glColor3f(1.0f, 1.0f, 1.0f);
	//glEnd();
}

bool ModuleRenderer3D::RenderSceneGeometry()
{
	bool ret = true;

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Geometry Render");

	//Bind buffer where we will draw
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

	glViewport(0, 0, imageSize.x, imageSize.y);

	//Clear the buffer from color and depth
	Color backgroundColor = engine->camera->background;
	//glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	/*for (std::map<float, GnMesh*>::reverse_iterator it = sceneMeshes.rbegin(); it != sceneMeshes.rend(); ++it)
	{
		it->second->Render();
	}*/

	for (size_t i = 0; i < meshesToRender.size(); ++i)
	{
		meshesToRender[i]->Render();
	}

	//Bind to default buffer to avoid drawing on top of the geometry in other processes
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	TakeScreenshot(sceneFramebuffer, "scene.png");

	glPopDebugGroup();

	return ret;
}

void ModuleRenderer3D::GenerateQuad()
{
	//Geometry
	glGenBuffers(1, &quad.embeddedVertices);
	glBindBuffer(GL_ARRAY_BUFFER, quad.embeddedVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad.vertices), quad.vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &quad.embeddedElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.embeddedElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad.indices), quad.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Attribute state
	glGenVertexArrays(1, &quad.vao);
	glBindVertexArray(quad.vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad.embeddedVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexV3V2), (void*)12);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.embeddedElements);
	glBindVertexArray(0);
}

//TODO: Extract it into a Mesh Resource
void ModuleRenderer3D::RenderQuad()
{
	if (!quadShader)
	{
		quadShader = (ResourceShader*)engine->resources->RequestResource(engine->resources->Find("Assets/Shaders/deferred_quad.vert"));
	}

	if (quadShader)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Quad Render");

		//Bind buffer where we will draw
		glBindFramebuffer(GL_FRAMEBUFFER, quadFramebuffer);
	
		glViewport(0, 0, imageSize.x, imageSize.y);
		
		Color backgroundColor = engine->camera->background;
		//glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		quadShader->Use();
		glBindVertexArray(quad.vao);

		//Set all textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedoAttachmentHandle);
		GLuint colorTextureLocation = glGetUniformLocation(quadShader->GetHandle(), "uColor");
		glUniform1i(colorTextureLocation, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalsAttachmentHandle);
		GLuint normalsTextureLocation = glGetUniformLocation(quadShader->GetHandle(), "uNormals");
		glUniform1i(normalsTextureLocation, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, positionAttachmentHandle);
		GLuint positionTextureLocation = glGetUniformLocation(quadShader->GetHandle(), "uPosition");
		glUniform1i(positionTextureLocation, 2);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);		
		glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		TakeScreenshot(quadFramebuffer, "quad.png");

		glPopDebugGroup();
	}
}

void ModuleRenderer3D::RenderEditor()
{
	//Clear main Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, engine->window->width, engine->window->height);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Draw the editor
	engine->editor->Draw();

	TakeScreenshot(0, "editor.png");
}

void ModuleRenderer3D::GenerateBuffers()
{
	//Scene FrameBuffer
	glGenFramebuffers(1, &sceneFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

	vec2 displaySize(engine->window->width, engine->window->height);

	GenerateColorTexture(albedoAttachmentHandle, displaySize, GL_RGBA8, GL_COLOR_ATTACHMENT0);
	GenerateColorTexture(normalsAttachmentHandle, displaySize, GL_RGBA16F, GL_COLOR_ATTACHMENT1);
	GenerateColorTexture(positionAttachmentHandle, displaySize, GL_RGBA16F, GL_COLOR_ATTACHMENT2);

	//Depth buffer
	GenerateDepthTexture(depthAttachmentHandle, displaySize);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(ARRAY_COUNT(attachments), attachments);

	//RenderBuffer
	//glGenRenderbuffers(1, &renderBuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, engine->window->width, engine->window->height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	CheckFramebufferStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Quad FrameBuffer
	//Generate a framebuffer for the quad
	glGenFramebuffers(1, &quadFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, quadFramebuffer);
	//Generate a texture to draw the quad
	GenerateColorTexture(finalRenderAttachmentHandle, displaySize, GL_RGBA16F, GL_COLOR_ATTACHMENT0);

	CheckFramebufferStatus();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

bool ModuleRenderer3D::CheckFramebufferStatus()
{
	bool ret = true;

	//Check for error
	GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (framebufferStatus)
		{
		case GL_FRAMEBUFFER_UNDEFINED:						LOG_ERROR("GL_FRAMEBUFFER_UNDEFINED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
		case GL_FRAMEBUFFER_UNSUPPORTED:					LOG_ERROR("GL_FRAMEBUFFER_UNSUPPORTED"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		LOG_ERROR("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
		default:											LOG_ERROR("Unknown framebuffer status error") break;
		}

		ret = false;
	}

	return ret;
}

void ModuleRenderer3D::GenerateColorTexture(GLuint& textureAttachmentHandle, vec2 displaySize, GLint internalFormat, GLuint glColorAttachment)
{
	glGenTextures(1, &textureAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, textureAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, displaySize.x, displaySize.y, 0, internalFormat, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, glColorAttachment, GL_TEXTURE_2D, textureAttachmentHandle, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer3D::GenerateDepthTexture(GLuint& newDepthAttachmentHandle, vec2 displaySize)
{
	glGenTextures(1, &newDepthAttachmentHandle);
	glBindTexture(GL_TEXTURE_2D, newDepthAttachmentHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, displaySize.x, displaySize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRenderer3D::BeginDebugDraw() {}

void ModuleRenderer3D::EndDebugDraw() {}

void ModuleRenderer3D::TakeScreenshot(int framebuffer, const char* name)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilutGLScreen();
	ilEnable(IL_FILE_OVERWRITE);
	std::string path = "Screenshots/";
	path += name;
	ilSaveImage(path.c_str());
	ilDeleteImage(imageID);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}