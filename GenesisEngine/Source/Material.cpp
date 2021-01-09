#include "Material.h"
#include "Mesh.h"
#include "ImGui/imgui.h"
#include "FileSystem.h"
#include "GnJSON.h"
#include "GameObject.h"
#include "Application.h"
#include "ResourceTexture.h"
#include "ResourceShader.h"
#include "glew/include/glew.h"
#include "ResourceMaterial.h"
#include "WindowAssets.h"
#include "WindowShaderEditor.h"
#include "Transform.h"
#include "Time.h"

Material::Material() : Component(), checkers_image(false), _resource(nullptr), colored(false), shader(nullptr)
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

Material::Material(GameObject* gameObject) : Component(gameObject), checkers_image(false), _resource(nullptr), colored(false)
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

	shader = dynamic_cast<ResourceShader*>(App->resources->RequestResource(App->resources->Find("Assets/Shaders/phong.vert")));

	if (shader == nullptr)
		shader = App->renderer3D->GetDefaultShader();

	shaderID = shader->GetUID();
	//shader = dynamic_cast<ResourceShader*>(App->resources->RequestResource(App->resources->Find("Assets/Shaders/water_shader.vert")));
}

Material::~Material()
{
	if (_resource != nullptr)
	{
		if (_resource->diffuseMap != nullptr)
		{
			App->resources->ReleaseResource(_resource->diffuseMap->GetUID());
			_resource->diffuseMap = nullptr;
		}

		if (_resource->normalMap != nullptr)
		{
			App->resources->ReleaseResource(_resource->normalMap->GetUID());
			_resource->normalMap = nullptr;
		}

		App->resources->ReleaseResource(_resourceUID);
		_resource = nullptr;
	}

	glDeleteTextures(1, &checkersID);
}

void Material::SetResourceUID(uint UID)
{
	//Release old material
	if (_resource != nullptr)
		App->resources->ReleaseResource(_resource->GetUID());

	//Assign new material
	_resourceUID = UID;
	_resource = dynamic_cast<ResourceMaterial*>(App->resources->RequestResource(UID));
}

void Material::SetShader(ResourceShader* new_shader)
{
	if (shader != nullptr)
		App->resources->ReleaseResource(shader->GetUID());

	if (new_shader != nullptr)
	{
		shader = new_shader;
		shaderID = new_shader->GetUID();
	}
	else
	{
		App->renderer3D->GetDefaultShader();}
}

void Material::BindTexture(ResourceTexture* texture)
{
	if (!App->resources->Exists(_resourceUID)) 
	{
		_resource = nullptr;
		_resourceUID = 0u;
		checkers_image = true;
	}
	else if(texture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, texture->GetGpuID());
	}

}

void Material::UseShader()
{
	if(App->resources->Exists(shaderID))
	{
		shader->Use();
		shader->UpdateUniforms(this, _resource);
	}
	else
	{
		shader = App->renderer3D->GetDefaultShader();
	}

	//diffuse map
	if (checkers_image == false)
	{
		if (_resource->diffuseMap != nullptr)
		{
			shader->SetInt("diffuseMap", 0);
			glActiveTexture(GL_TEXTURE0);
			BindTexture(_resource->diffuseMap);
			shader->SetVec2("diffuseMapTiling", _resource->tiling[DIFFUSE_MAP][0], _resource->tiling[DIFFUSE_MAP][1]);
			shader->SetBool("hasDiffuseMap", true);
		}
		else {
			shader->SetBool("hasDiffuseMap", false);}
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, checkersID);
	}

	//normal map
	if (_resource->normalMap != nullptr)
	{
		if (_resource->normalMap != nullptr)
		{
			shader->SetInt("normalMap", 1);
			glActiveTexture(GL_TEXTURE1);
			BindTexture(_resource->normalMap);
			shader->SetVec2("normalMapTiling", _resource->tiling[NORMAL_MAP][0], _resource->tiling[NORMAL_MAP][1]);
			shader->SetBool("hasNormalMap", true);
		}
		else {
			shader->SetBool("hasNormalMap", false);}
	}

	
	shader->SetInt("depthMap", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, App->renderer3D->depthTexture);
	
}

void Material::Save(GnJSONArray& save_array)
{
	GnJSONObj save_object;

	save_object.AddInt("Type", type);
	save_object.AddInt("MaterialID", _resource->GetUID());
	if (shader != nullptr)
		save_object.AddInt("ShaderID", shader->GetUID());

	save_array.AddObject(save_object);
}

void Material::Load(GnJSONObj& load_object)
{
	_resourceUID = load_object.GetInt("MaterialID", 0);
	_resource = (ResourceMaterial*)App->resources->RequestResource(_resourceUID);
	uint shaderUID = load_object.GetInt("ShaderID", 0);
	SetShader((ResourceShader*)App->resources->RequestResource(shaderUID));
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
		}

		ImGui::Text("Material UID: %d", _resourceUID);

		ImGui::Separator();

		_resource->diffuseMap = DrawTextureInformation(_resource->diffuseMap, TextureType::DIFFUSE_MAP);

		ImGui::Separator();

		_resource->normalMap = DrawTextureInformation(_resource->normalMap, TextureType::NORMAL_MAP);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::Button(shader->name.c_str());

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int UID = *(const int*)payload->Data;

				Resource* possible_shader = App->resources->RequestResource(UID);

				if (possible_shader != nullptr && possible_shader->GetType() == ResourceType::RESOURCE_SHADER) {
					SetShader(dynamic_cast<ResourceShader*>(App->resources->RequestResource(UID)));

					//Update shader
					WindowShaderEditor* shaderEditor = dynamic_cast<WindowShaderEditor*>(App->editor->windows[WINDOW_SHADER_EDITOR]);
					if (shaderEditor->visible)
						shaderEditor->Open(shader->assetsFile.c_str());
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if(ImGui::Button("Open Shader editor")) {
			WindowShaderEditor* shaderEditor = dynamic_cast<WindowShaderEditor*>(App->editor->windows[WINDOW_SHADER_EDITOR]);
			shaderEditor->Open(shader->assetsFile.c_str());
		}

		ImGui::Spacing();
		ImGui::Spacing();

		shader->OnEditor(this, _resource);
	}
}

ResourceTexture* Material::DrawTextureInformation(ResourceTexture* texture, TextureType type)
{
	if (texture != nullptr)
	{
		if (type == TextureType::DIFFUSE_MAP)
			ImGui::Text("Diffuse Texture: %s", texture->name.c_str());
		else if (type == TextureType::NORMAL_MAP)
			ImGui::Text("Normal Map: %s", texture->name.c_str());

		ImGui::Text("Width: %d Height: %d", texture->GetWidth(), texture->GetHeight());
	}
	else
	{
		ImGui::Text("No Texture");
	}

	ImGui::Spacing();

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 70.0f);
	if (texture != nullptr) {
		ImGui::Image((ImTextureID)texture->GetGpuID(), ImVec2(60, 60), ImVec2(0, 1), ImVec2(1, 0));}
	else {
		ImGui::Image((ImTextureID)checkersID, ImVec2(60, 60), ImVec2(0, 1), ImVec2(1, 0));}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int payload_n = *(const int*)payload->Data;
			WindowAssets* assets_window = (WindowAssets*)App->editor->windows[WINDOW_ASSETS];
			Resource* possible_texture = App->resources->RequestResource(payload_n);

			if (possible_texture->GetType() == ResourceType::RESOURCE_TEXTURE) {
				texture = dynamic_cast<ResourceTexture*>(possible_texture);
				texture->type = type;
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::NextColumn();
	if (texture != nullptr) 
	{
		//ImGui::SameLine();

		std::string button_text;
		if (type == TextureType::DIFFUSE_MAP)
			button_text = "Remove Diffuse Map";
		else if (type == TextureType::NORMAL_MAP)
			button_text = "Remove Normal Map";

		if (ImGui::Button(button_text.c_str()))
		{
			App->resources->ReleaseResource(texture->GetUID());
			texture = nullptr;
		}
		else
		{
			ImGui::PushID(texture->GetUID());
			ImGui::DragFloat2("Tiling", _resource->tiling[type], 0.05, -1000.0, 1000.0);
			ImGui::PopID();
		}
	}

	ImGui::Columns(1);

	ImGui::Spacing();
	if (type == TextureType::DIFFUSE_MAP)
	{
		ImVec4 color = ImVec4(_resource->diffuseColor.r, _resource->diffuseColor.g, _resource->diffuseColor.b, _resource->diffuseColor.a);
		if (ImGui::ColorEdit4("Albedo", (float*)&color)) {
			_resource->diffuseColor = Color(color.x, color.y, color.z, color.w);
		}
	}

	if (texture != nullptr)
	{
		ImGui::Spacing();
		ImGui::Text("Assets path: %s", texture->assetsFile.c_str());
		ImGui::Text("Library path: %s", texture->libraryFile.c_str());
	}

	return texture;
}

void Material::SetTexture(ResourceTexture* texture, TextureType type)
{
	if (texture != nullptr)
	{
		switch (type)
		{
		case TextureType::DIFFUSE_MAP:
			_resource->diffuseMap = texture;
			_resource->diffuseMapID = texture->GetID();
			break;
		case TextureType::NORMAL_MAP:
			_resource->normalMap = texture;
			_resource->normalMapID = texture->GetID();
			break;
		case TextureType::UNKNOWN_MAP:
			LOG_ERROR("Trying to apply unwnown texture type: %s", texture->name);
			break;
		default:
			break;
		}
	}
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
	//return _diffuseMap;
	return nullptr;
}
