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
#include "WindowAssets.h"

Material::Material() : Component(), checkers_image(false), _resource(nullptr) 
{
	type = ComponentType::MATERIAL;
}

Material::Material(GameObject* gameObject) : Component(gameObject), checkers_image(false), _resource(nullptr), _diffuseTexture(nullptr)
{
	type = ComponentType::MATERIAL;
}

Material::~Material()
{
	if (_resource != nullptr)
	{
		//DeleteTexture();
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

}

void Material::BindTexture()
{
	if(!checkers_image)
		_diffuseTexture->BindTexture();
	else 
		glBindTexture(GL_TEXTURE_2D, checkersID);
}

void Material::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("Material UID", _resource->GetUID());

	if (_diffuseTexture != nullptr)
		save_object.AddInt("Texture UID", _diffuseTexture->GetUID());

	//if (diffuseTexture != nullptr)
		//save_object.AddString("Path", diffuse_texture->path.c_str());

	save_array.AddObject(save_object);
}

void Material::Load(GnJSONObj& load_object)
{
	uint materialUID = load_object.GetInt("Material UID");
	_resource = (ResourceMaterial*)App->resources->RequestResource(materialUID);

	uint textureUID = load_object.GetInt("Texture UID", -1);
	_resource->diffuseTextureUID = textureUID;

	if (textureUID != -1) {
		_diffuseTexture = (ResourceTexture*)App->resources->RequestResource(textureUID);
		GenerateTextureBuffers();
	}

	if (_diffuseTexture == nullptr)
		AssignCheckersImage();
	else
		SetTexture(_diffuseTexture);
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

			ImGui::Text("Width: %d Height: %d", _diffuseTexture->GeWidth(), _diffuseTexture->GetHeight());

			ImGui::Spacing();

			ImGui::Image((ImTextureID)_diffuseTexture->GetID(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					WindowAssets* assets_window = (WindowAssets*)App->editor->windows[ASSETS_WINDOW];
					const char* file = assets_window->GetFileAt(payload_n);
					Resource* possible_texture = App->resources->RequestResource(App->resources->Find(file));

					if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE)
						_diffuseTexture = (ResourceTexture*)possible_texture;
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::Button("Remove Texture"))
			{
				_diffuseTexture = nullptr;
				AssignCheckersImage();
			}
		}
	}
}

void Material::SetTexture(ResourceTexture* texture)
{
	//if (_diffuseTexture != nullptr)
		//DeleteTexture();

	_diffuseTexture = texture;
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

	glBindTexture(GL_TEXTURE_2D, checkersID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkers_image = true;
}

ResourceTexture* Material::GetDiffuseTexture()
{
	return _diffuseTexture;
	return nullptr;
}
