#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "parson/parson.h"

#include "glew/include/glew.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
	name = "scene";
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));

	App->camera->LookAt(vec3(0, 0, 0));
	return ret;
}

bool ModuleSceneIntro::Init(JSON_Object* object)
{
	show_grid = json_object_get_boolean(object, "show_grid");
	return true;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	bool wired = (App->renderer3D->GetDisplayMode() == WIREFRAME);

	if (show_grid) 
	{
		Plane p(0, 1, 0, 0);
		//p.axis = true;
		p.Render(wired);
	}
	
	/*
	Cube cube(1.0f, 1.0f, 1.0f);
	cube.color = Color(255, 0, 0, 255);
	cube.wire = true;
	cube.Render(wired);
	*/

	PracticeOpenGL();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::PracticeOpenGL()
{
	float vertices[24] = 
	{
		0,0,0,
		1,0,0,
		1,0,1,
		0,0,1,

		0,1,0,
		1,1,0,
		1,1,1,
		0,1,1
	};

	uint indices[36] =
	{
		//Bottom face
		0,1,2, 2,3,0,
		//Front Face
		3,2,6, 6,7,3,
		//Right face
		6,2,1, 1,5,6,
		//Left face
		4,0,7, 7,0,3,
		//Back face
		1,0,5, 0,4,5,
		//Top face
		4,7,6, 6,5,4
	};

	uint vertices_buffer = 0;
	glGenBuffers(1, (GLuint*)&(vertices_buffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	uint indices_buffer = 0;
	glGenBuffers(1, (GLuint*)&(indices_buffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

