#include "Mesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "FileSystem.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"
#include "GnJSON.h"

#include "ResourceMesh.h"

#include "glew/include/glew.h"
#include "ImGui/imgui.h"

// GnMesh =========================================================================================================================

GnMesh::GnMesh() : Component(), draw_face_normals(false), draw_vertex_normals(false), name("No name"), _resource(nullptr)
{
	type = ComponentType::MESH;
}

GnMesh::~GnMesh() 
{
	if (_resource != nullptr) 
	{
		App->resources->ReleaseResource(_resource->GetUID());
		_resource = nullptr;
	}
}

void GnMesh::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("MeshID", _resource->GetUID());

	save_array.AddObject(save_object);
}

void GnMesh::Load(GnJSONObj& load_object)
{
	int meshUID = load_object.GetInt("MeshID");
	SetResourceUID(meshUID);
}

void GnMesh::SetResourceUID(uint UID)
{
	if (_resource != nullptr)
		App->resources->ReleaseResource(_resourceUID);

	_resourceUID = UID;
	_resource = (ResourceMesh*)App->resources->RequestResource(_resourceUID);
	if(_resource != nullptr)
		GenerateAABB();
}

Resource* GnMesh::GetResource(ResourceType type)
{
	return _resource;
}

void GnMesh::GenerateAABB()
{
	_AABB.SetNegativeInfinity();
	_AABB.Enclose((float3*)_resource->vertices, _resource->vertices_amount);
}

AABB GnMesh::GetAABB()
{
	return _AABB;
}

void GnMesh::Update()
{
 	Render();
}

void GnMesh::Render()
{
	if (!App->resources->Exists(_resourceUID)) {
		_resource = nullptr;
		_resourceUID = 0u;
		_AABB.SetNegativeInfinity();
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, _resource->vertices_buffer);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, _resource->normals_buffer);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//textures
	glBindBuffer(GL_ARRAY_BUFFER, _resource->texcoords_buffer);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _resource->indices_buffer);

	glPushMatrix();
	glMultMatrixf((float*)&_gameObject->GetTransform()->GetGlobalTransform().Transposed());

	Material* material = dynamic_cast<Material*>(_gameObject->GetComponent(ComponentType::MATERIAL));

	if (material != nullptr)
		material->BindTexture();

	glDrawElements(GL_TRIANGLES, _resource->indices_amount, GL_UNSIGNED_INT, NULL);

	if(draw_vertex_normals ||App->renderer3D->draw_vertex_normals)
		DrawVertexNormals();

	if (draw_face_normals || App->renderer3D->draw_face_normals)
		DrawFaceNormals();

	//App->renderer3D->DrawAABB(_AABB);

	glPopMatrix();

	//clean buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void GnMesh::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(" Enabled", &enabled);

		ImGui::Separator();
		ImGui::Spacing();

		if (_resource != nullptr)
		{
			std::string meshID = "Mesh: ";
			meshID.append(_resource->name);
			ImGui::Button(meshID.c_str());

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESHES"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					SetResourceUID(payload_n);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Spacing();

			ImGui::Text("Assets path: %s", _resource->assetsFile.c_str());
			ImGui::Text("Library path: %s", _resource->libraryFile.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Vertices: %d Indices: %d", _resource->vertices_amount, _resource->indices_amount);
			ImGui::Spacing();

			ImGui::Spacing();

			ImGui::Checkbox("Vertex Normals", &draw_vertex_normals);
			ImGui::SameLine();
			ImGui::Checkbox("Face Normals", &draw_face_normals);
		}
		else
		{
			ImGui::Button("Unknown Mesh");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESHES"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					SetResourceUID(payload_n);
				}
				ImGui::EndDragDropTarget();
			}
		}


		ImGui::Spacing();
		ImGui::Text("UID: %d", _resourceUID);
		ImGui::Spacing();
	}
}

void GnMesh::DrawVertexNormals()
{
	if (_resource->normals_buffer == -1)
		return;

	float normal_lenght = 0.5f;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < _resource->vertices_amount * 3; i += 3, c+= 4)
	{
		glColor3f(0.0f, 0.85f, 0.85f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(_resource->vertices[i], _resource->vertices[i + 1], _resource->vertices[i + 2]);

		glVertex3f(_resource->vertices[i] + (_resource->normals[i] * normal_lenght),
			       _resource->vertices[i + 1] + (_resource->normals[i + 1] * normal_lenght),
			       _resource->vertices[i + 2] + (_resource->normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void GnMesh::DrawFaceNormals()
{
	if (_resource->normals_buffer == -1)
		return;

	float normal_lenght = 0.5f;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0; i < _resource->vertices_amount * 3; i += 3)
	{
		glColor3f(1.0f, 0.85f, 0.0f);
		float vx = (_resource->vertices[i] + _resource->vertices[i + 3] + _resource->vertices[i+ 6]) / 3;
		float vy = (_resource->vertices[i + 1] + _resource->vertices[i + 4] + _resource->vertices[i + 7]) / 3;
		float vz = (_resource->vertices[i + 2] + _resource->vertices[i + 5] + _resource->vertices[i + 8]) / 3;

		float nx = (_resource->normals[i] +     _resource->normals[i + 3] + _resource->normals[i + 6]) / 3;
		float ny = (_resource->normals[i + 1] + _resource->normals[i + 4] + _resource->normals[i + 7]) / 3;
		float nz = (_resource->normals[i + 2] + _resource->normals[i + 5] + _resource->normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (_resource->normals[i] * normal_lenght),
			       vy + (_resource->normals[i + 1] * normal_lenght),
			       vz + (_resource->normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}


// --------------------------------------------------------------------------------------------------------------------------------

// GnCube =========================================================================================================================

GnCube::GnCube() : GnMesh()
{
	name = {"Cube"};

	////vertices = new float[24] 
	//{
	//	//Bottom Vertices
	//	0.0f ,0.0f, 0.0f,
	//	1.0f ,0.0f, 0.0f,
	//	1.0f ,0.0f, 1.0f,
	//	0.0f ,0.0f, 1.0f,

	//	//Top Vertices
	//	0.0f, 1.0f, 0.0f,
	//	1.0f, 1.0f, 0.0f,
	//	1.0f, 1.0f, 1.0f,
	//	0.0f, 1.0f, 1.0f
	//};

	//indices = new uint[36] 
	//{
	//	//Bottom face
	//	0,1,2, 2,3,0,
	//	//Front Face
	//	3,2,6, 6,7,3,
	//	//Left face
	//	7,4,0, 0,3,7,
	//	//Right face
	//	2,1,5, 5,6,2,
	//	//Back face
	//	1,0,4, 4,5,1,
	//	//Top face
	//	5,4,7, 7,6,5
	//};

	//texcoords = new float[16]
	//{
	//	0.0f, 0.0f, 
	//	1.0f, 0.0f,
	//	1.0f, 1.0f, 
	//	0.0f, 1.0f,

	//	0.0f, 1.0f,
	//	1.0f, 1.0f,
	//	1.0f, 0.0f,
	//	0.0f, 0.0f
	//};

	//vertices_amount = 8;
	//indices_amount = 36;

	//GenerateBuffers();
}

GnCube::~GnCube(){}

// --------------------------------------------------------------------------------------------------------------------------------

// GnPlane ========================================================================================================================

GnPlane::GnPlane() : GnMesh() 
{
	/*name = { "Plane" };

	vertices = new float[12]{
	0.0f ,0.0f, 0.0f,
	1.0f ,0.0f, 0.0f,
	1.0f ,0.0f, 1.0f,
	0.0f ,0.0f, 1.0f,
	};

	indices = new uint[6]{
		0, 3, 2,
		2, 1 ,0
	};

	vertices_amount = 4;
	indices_amount = 6;*/

	//GenerateBuffers();
}

GnPlane::~GnPlane() {}

// --------------------------------------------------------------------------------------------------------------------------------

// GnPyramid ======================================================================================================================

GnPyramid::GnPyramid() : GnMesh() 
{
	name = { "Pyramid" };

	//vertices = new float[15] {
	//	//Top
	//	0.5f, 0.85f, 0.5f,

	//	//Bottom 
	//	0.0f ,0.0f, 0.0f,
	//	1.0f ,0.0f, 0.0f,
	//	1.0f ,0.0f, 1.0f,
	//	0.0f ,0.0f, 1.0f
	//};

	//indices = new uint[18] {
	//	0, 4, 3, // Front
	//	0, 3, 2, // Left
	//	0, 2, 1, // Right
	//	0, 1, 4,  // Back

	//	1, 3, 4,  1, 2, 3 //Bottom
	//};

	//vertices_amount = 5;
	//indices_amount = 18;

	//GenerateBuffers();
}

GnPyramid::~GnPyramid() {}

// --------------------------------------------------------------------------------------------------------------------------------

// GnSphere =======================================================================================================================

GnSphere::GnSphere() : GnMesh() 
{
	/*name = { "Sphere" };

	float radius = 1;
	unsigned int rings = 12;
	unsigned int sectors = 24;

	float const R = 1.0f / (float)(rings - 1);
	float const S = 1.0f / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = sin( -M_PI * 0.5f + M_PI * r * R);
			float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2*M_PI * s * S) * sin(M_PI * r * R);

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}
	}

	indices.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = indices.begin();

	for (r = 0; r < rings -1; r++)
	{
		for (s = 0; s < sectors -1; s++)
		{
			*i++ = (r + 1) * sectors + s;
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = r * sectors + (s + 1);
			*i++ = r * sectors + s;
		}
	}*/
}

GnSphere::~GnSphere()
{
	//vertices.clear();
	//indices.clear();
}

void GnSphere::Render()
{
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	//glDrawElements(GL_QUADS, indices.size() , GL_UNSIGNED_SHORT, &indices[0]);
	//glDisableClientState(GL_VERTEX_ARRAY);
}

// -----------------------------------------------------------------------------------------------------------------------------

// GnCylinder ==================================================================================================================

GnCylinder::GnCylinder() : GnMesh(), radius(1), height(2), sides(16)  
{
	name = { "Cylinder" };
	CalculateGeometry();
}

GnCylinder::GnCylinder(float g_radius, float g_height, int g_sides) : GnMesh(), radius(g_radius), height(g_height), sides(g_sides) 
{
	CalculateGeometry();
}

GnCylinder::~GnCylinder() {}

void GnCylinder::CalculateGeometry()
{
	//float current_angle = 0;
	//float angle_increase = 2 * M_PI / sides;

	////Vertices ------------------------------------------------

	//std::vector<float> vertices_vector;

	////Top center
	//vertices_vector.push_back(0);
	//vertices_vector.push_back(height * 0.5f);
	//vertices_vector.push_back(0);

	////Top face
	//for (int i = 0; i < sides; i++)
	//{
	//	vertices_vector.push_back(radius * cos(current_angle));//x
	//	vertices_vector.push_back(height * 0.5f);		       //y
	//	vertices_vector.push_back(radius * sin(current_angle));//z

	//	//anticlockwise
	//	current_angle -= angle_increase;
	//}

	//current_angle = 0;

	////Bottom Center
	//vertices_vector.push_back(0);
	//vertices_vector.push_back(-height * 0.5f);
	//vertices_vector.push_back(0);

	////Bottom face
	//for (int i = 0; i < sides; i++)
	//{
	//	vertices_vector.push_back(radius * cos(current_angle)); //x
	//	vertices_vector.push_back(-height * 0.5f);			    //y
	//	vertices_vector.push_back(radius * sin(current_angle)); //z

	//	//clockwise
	//	current_angle -= angle_increase;
	//}

	//// Indices ----------------------------------------------

	//std::vector<uint> indices_vector;

	////Top Face
	//for (int i = 1; i < sides; i++)
	//{
	//	indices_vector.push_back(0);
	//	indices_vector.push_back(i);
	//	indices_vector.push_back(i + 1);
	//}

	//indices_vector.push_back(0);
	//indices_vector.push_back(sides);
	//indices_vector.push_back(1);

	////Sides
	//for (int i = 1; i < sides; i++)
	//{
	//	//Left triangle
	//	indices_vector.push_back(i);
	//	indices_vector.push_back(sides + i + 1);
	//	indices_vector.push_back(sides + i + 2);

	//	//Right triangle
	//	indices_vector.push_back(i + sides + 2);
	//	indices_vector.push_back(i + 1);
	//	indices_vector.push_back(i);
	//}

	//indices_vector.push_back(sides);
	//indices_vector.push_back(2 * sides + 1);
	//indices_vector.push_back(sides + 2);

	//indices_vector.push_back(sides + 2);
	//indices_vector.push_back(1);
	//indices_vector.push_back(sides);

	////Bottom Face
	//int k = sides + 1;
	//for (int i = 1; i < sides; i++)
	//{
	//	indices_vector.push_back(k);
	//	indices_vector.push_back(k + i + 1);
	//	indices_vector.push_back(k + i);
	//}

	//indices_vector.push_back(k);
	//indices_vector.push_back(sides + 2);
	//indices_vector.push_back(2 * sides + 1);

	//vertices_amount = vertices_vector.size();
	//vertices = new float[vertices_amount]();

	//for (size_t i = 0; i < vertices_amount; i++)
	//{
	//	vertices[i] = vertices_vector[i];
	//}

	//indices_amount = indices_vector.size();
	//indices = new uint[indices_amount]();

	//for (size_t i = 0; i < indices_amount; i++)
	//{
	//	indices[i] = indices_vector[i];
	//}

	//vertices_vector.clear();
	//indices_vector.clear();

	//GenerateBuffers();
}

// --------------------------------------------------------------------------------------------------------------------------------

// GnGrid =========================================================================================================================

GnGrid::GnGrid(int g_size) 
{
	if ((g_size % 2) != 0)
		g_size++;

	size = g_size;
}

GnGrid::~GnGrid() {}

void GnGrid::Render()
{
	glBegin(GL_LINES);

	//Vertical Lines
	for (float x = -size * 0.5f; x <= size * 0.5f; x++)
	{	
		glVertex3f(x, 0, -size * 0.5f);
		glVertex3f(x, 0, size * 0.5f);
	}

	//Hortiontal Lines
	for (float z = -size * 0.5f; z <= size * 0.5f; z++)
	{
		glVertex3f(-size * 0.5f, 0, z);
		glVertex3f(size * 0.5f, 0, z);
	}

	glEnd();
}

// --------------------------------------------------------------------------------------------------------------------------------

// GnCone =========================================================================================================================

GnCone::GnCone() : GnMesh(), radius(1), height(1) 
{
	name = { "Cone" };
	CalculateGeometry(8);
}

GnCone::GnCone(float g_radius, float g_height, int sides) : GnMesh(), radius(g_radius) , height(g_height)
{
	name = { "Cone" };
	CalculateGeometry(sides);
}

GnCone::~GnCone() {}

void GnCone::CalculateGeometry(int sides) 
{
	////Verices -------------------------------------------------------
	//std::vector<GLfloat> vertices_vector;

	////Top vertex
	//vertices_vector.push_back(0);
	//vertices_vector.push_back(height * 0.5f);
	//vertices_vector.push_back(0);

	//float current_angle = 0;
	//float angle_increment = 2 * M_PI / sides;

	////Circle vertices
	//for (size_t i = 0; i < sides; i++)
	//{
	//	vertices_vector.push_back(radius * cos(current_angle)); //x
	//	vertices_vector.push_back(-height * 0.5f);			    //y
	//	vertices_vector.push_back(radius * sin(current_angle)); //z

	//	//clockwise
	//	current_angle -= angle_increment;
	//}

	////Circle center Vertex
	//vertices_vector.push_back(0);
	//vertices_vector.push_back(-height * 0.5f);
	//vertices_vector.push_back(0);

	////Indices -------------------------------------------------------

	//std::vector<GLuint> indices_vector;

	////Sides
	//for (size_t i = 0; i < sides; i++)
	//{
	//	indices_vector.push_back(0);
	//	indices_vector.push_back(i);
	//	indices_vector.push_back(i + 1);
	//}
	////Last Side
	//indices_vector.push_back(0);
	//indices_vector.push_back(sides);
	//indices_vector.push_back(1);

	////Bottom Face
	//for (size_t i = 1; i < sides; i++)
	//{
	//	indices_vector.push_back(i + 1);
	//	indices_vector.push_back(i);
	//	indices_vector.push_back(sides + 1);
	//}

	//indices_vector.push_back(1);
	//indices_vector.push_back(sides);
	//indices_vector.push_back(sides + 1);

	////Copy into default class containers

	////vertices
	//vertices_amount = vertices_vector.size();
	//vertices = new float[vertices_amount]();

	//for (size_t i = 0; i < vertices_amount; i++)
	//{
	//	vertices[i] = vertices_vector[i];
	//}

	//indices_amount = indices_vector.size();
	//indices = new uint[indices_amount]();

	//for (size_t i = 0; i < indices_amount; i++)
	//{
	//	indices[i] = indices_vector[i];
	//}

	//vertices_vector.clear();
	//indices_vector.clear();

	//GenerateBuffers();
}

