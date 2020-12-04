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

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Material::Material(GameObject* gameObject) : Component(gameObject), checkers_image(false), _resource(nullptr), _diffuseTexture(nullptr)
{
	type = ComponentType::MATERIAL;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Material::~Material()
{
	if (_resource != nullptr)
	{
		App->resources->ReleaseResource(_resourceUID);
		_resource = nullptr;

		if (_diffuseTexture != nullptr)
		{
			App->resources->ReleaseResource(_diffuseTexture->GetUID());
			_diffuseTexture = nullptr;
		}
	}
}

void Material::Update() {}

void Material::SetResourceUID(uint UID)
{
	_resourceUID = UID;
	_resource = dynamic_cast<ResourceMaterial*>(App->resources->RequestResource(UID));

	if (_resource->diffuseTextureUID != 0)
	{
		if (_diffuseTexture != nullptr)
			App->resources->ReleaseResource(_diffuseTexture->GetUID());

		_diffuseTexture = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(_resource->diffuseTextureUID));
	}

	if (_diffuseTexture == nullptr)
		AssignCheckersImage();
	else
		SetTexture(_diffuseTexture);
}

void Material::BindTexture()
{
	//TODO: Request resource every frame
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
		//GenerateTextureBuffers();
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

		if(_diffuseTexture != nullptr && checkers_image == false)
		{
			ImGui::Text("Texture: %s", _diffuseTexture->assetsFile.c_str());
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
		else
		{
			ImGui::Image((ImTextureID)checkersID, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

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
						SetTexture(dynamic_cast<ResourceTexture*>(possible_texture));
				}
				ImGui::EndDragDropTarget();
			}
		}
	}
}

void Material::SetTexture(ResourceTexture* texture)
{
	_diffuseTexture = texture;
	checkers_image = false;
}

void Material::AssignCheckersImage()
{
	int CHECKERS_WIDTH = 64;
	int CHECKERS_HEIGHT = 64;

	GLubyte checkerImage[64][64][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x4) == 0) ^ (((j & 0x4)) == 0))) * 255;
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
}
