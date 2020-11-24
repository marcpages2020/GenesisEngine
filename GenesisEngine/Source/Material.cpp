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

Material::Material() : Component(), checkers_image(false), _resource(nullptr) 
{
	type = ComponentType::MATERIAL;
}

Material::Material(GameObject* gameObject) : Component(gameObject), checkers_image(false), _resource(nullptr)
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

void Material::Update() {}

void Material::SetResourceUID(uint UID)
{
	_resource = (ResourceMaterial*)App->resources->RequestResource(UID);
	_diffuseTexture = (ResourceTexture*)App->resources->RequestResource(_resource->diffuseTextureUID);

	GenerateTextureBuffers();

	if (_diffuseTexture == nullptr)
		AssignCheckersImage();
	else
		SetTexture(_diffuseTexture);
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

void Material::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Material::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddString("Resource UID", _resource->libraryFile.c_str());

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
			if (checkers_image)
			{
				AssignCheckersImage();
			}
			else
			{
				if (_diffuseTexture != nullptr)
					SetTexture(_diffuseTexture);
				else
					checkers_image = true;
			}


		}

		ImGui::Separator();

		if(_diffuseTexture != nullptr)
		{
			//ImGui::Text("Diffuse Texture: %s", _diffuseTexture->name.c_str());
			//ImGui::Text("Path: %s", diffuse_texture->path.c_str());

			ImGui::Text("Width: %d Height: %d", _diffuseTexture->width, _diffuseTexture->height);

			ImGui::Spacing();
			ImGui::Image((ImTextureID)_diffuseTexture->id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::Button("Remove Texture"))
			{
				DeleteTexture();
				//mesh->RemoveTexture();
			}
		}
	}
}

void Material::SetTexture(ResourceTexture* texture)
{
	//if (_diffuseTexture != nullptr)
		//DeleteTexture();

	_diffuseTexture = texture;

	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _diffuseTexture->width, _diffuseTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _diffuseTexture->data);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);
}

ResourceTexture* Material::GetDiffuseTexture()
{
	return _diffuseTexture;
	return nullptr;
}

bool Material::DeleteTexture()
{
	bool ret = true;

	glDeleteTextures(1, &_textureID);

	//TODO
	//TextureImporter::UnloadTexture(_diffuseTextureID);
	//delete diffuse_texture;
	//diffuse_texture = nullptr;

	return ret;
}
