#include "Mesh.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "FileSystem.h"
#include "Material.h"
#include "GameObject.h"
#include "Transform.h"
#include "GnJSON.h"

#include "ResourceMesh.h"
#include "ResourceShader.h"

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

	Material* material = dynamic_cast<Material*>(_gameObject->GetComponent(ComponentType::MATERIAL));
	if (material != nullptr)
	{
		material->BindTexture();

		GLint modelLoc = glGetUniformLocation(material->shader->id, "model_matrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _gameObject->GetTransform()->GetGlobalTransform().Transposed().ptr());

		material->UseShader();
	}

	//vertices
	glBindVertexArray(_resource->VAO);

	//normals
	//glBindBuffer(GL_NORMAL_ARRAY, _resource->normals_buffer);
	//glNormalPointer(GL_FLOAT, 0, NULL);

	//indices
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _resource->EBO);
	//glDrawElements(GL_TRIANGLES, _resource->indices_amount, GL_UNSIGNED_INT, NULL);

	/*if(draw_vertex_normals ||App->renderer3D->draw_vertex_normals)
		DrawVertexNormals();

	if (draw_face_normals || App->renderer3D->draw_face_normals)
		DrawFaceNormals();*/

	//App->renderer3D->DrawAABB(_AABB);

	//clean buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glBindVertexArray(0);
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

