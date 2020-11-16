#include "Material.h"
#include "Mesh.h"
#include "ImGui/imgui.h"
#include "FileSystem.h"
#include "GnJSON.h"
#include "GameObject.h"
#include "Application.h"
#include "ResourceTexture.h"
#include "glew/include/glew.h"
#include "ResourceMaterial.h"

Material::Material() : Component(), checkers_image(true), _resource(nullptr) {
	type = ComponentType::MATERIAL;
}

Material::Material(GameObject* gameObject) : Component(gameObject), checkers_image(true), _resource(nullptr)
{
	type = ComponentType::MATERIAL;
}

Material::~Material()
{
	if (_resource != nullptr)
	{
		DeleteTexture();
	}
}

void Material::Update()
{

}

void Material::SetResourceUID(uint UID)
{
	_resource = (ResourceMaterial*)App->resources->RequestResource(UID);
	_diffuseTexture = (ResourceTexture*)App->resources->RequestResource(_resource->diffuseTextureUID);
	GenerateTextureBuffers();
}

void Material::GenerateTextureBuffers()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("Type", type);

	//if (diffuseTexture != nullptr)
		//save_object.AddString("Path", diffuse_texture->path.c_str());

	save_array.AddObject(save_object);
}

void Material::Load(GnJSONObj& load_object)
{
	//TODO
	//MaterialImporter::Load(load_object.GetString("Path"), this);

	//if (diffuse_texture != nullptr && mesh != nullptr)
		//mesh->SetTexture(diffuse_texture);
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Enabled", &enabled)) {}

		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers Image", &checkers_image))
		{
			//if (diffuse_texture != nullptr)
			//{
			//	if (checkers_image == false)
			//		mesh->SetTexture(diffuse_texture);
			//	else
			//		mesh->AssignCheckersImage();
			//}
			//else
			//{
			//	checkers_image = true;
			//}
		}

		//ImGui::Separator();

		/*
		if(diffuse_texture != nullptr)
		{
			ImGui::Text("Diffuse Texture: %s", diffuse_texture->name.c_str());
			ImGui::Text("Path: %s", diffuse_texture->path.c_str());

			ImGui::Text("Width: %d Height: %d", diffuse_texture->width, diffuse_texture->height);

			ImGui::Spacing();
			ImGui::Image((ImTextureID)diffuse_texture->id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::Button("Remove Texture"))
			{
				DeleteTexture();

				mesh->RemoveTexture();
			}
		}
		*/
	}
}

void Material::SetTexture(ResourceTexture* texture)
{
	/*if (diffuseTexture != nullptr)
		DeleteTexture();*/

	//diffuseTexture = (ResourceMaterial*)App->resources->RequestResource(_resourceUID);

	//diffuse_texture = texture;

	/*if (mesh != nullptr)
	{
		checkers_image = !mesh->SetTexture(diffuse_texture);
	}*/
}

void Material::AssignCheckersImage()
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

	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

ResourceTexture* Material::GetDiffuseTexture()
{
	//return diffuse_texture;
	return nullptr;
}

void Material::SetMesh(GnMesh* g_mesh)
{
	//mesh = g_mesh;

	//if (diffuse_texture != nullptr)
	//{
	//	checkers_image = !mesh->SetTexture(diffuse_texture);
	//}
}

bool Material::DeleteTexture()
{
	bool ret = true;

	//TODO
	//TextureImporter::UnloadTexture(diffuse_texture->id);
	//delete diffuse_texture;
	//diffuse_texture = nullptr;

	return ret;
}
