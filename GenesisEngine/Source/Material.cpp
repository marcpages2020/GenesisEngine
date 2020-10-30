#include "Material.h"
#include "Mesh.h"
#include "ImGui/imgui.h"
#include "FileSystem.h"

Material::Material() : Component(), mesh(nullptr), checkers_image(true), diffuse_texture(nullptr) {
	type = ComponentType::MATERIAL;
}

Material::Material(GnMesh* g_mesh, GnTexture* g_diffuse_texture) : Component(), checkers_image(true), diffuse_texture(nullptr)
{
	mesh = g_mesh;
	type = ComponentType::MATERIAL;

	if (g_diffuse_texture->data != nullptr)
	{
		SetTexture(g_diffuse_texture);
		checkers_image = false;
	}
	else
	{
		mesh->AssignCheckersImage();
	}
}

Material::~Material()
{
	if (diffuse_texture != nullptr)
	{
		DeleteTexture();
	}

	mesh = nullptr;
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Enabled", &enabled)) {
			LOG("Checked");
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers Image", &checkers_image))
		{
			if (checkers_image == false)
				mesh->SetTexture(diffuse_texture);
			else
				mesh->AssignCheckersImage();
		}

		ImGui::Separator();

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

		ImGui::Spacing();
	}
}

void Material::SetTexture(GnTexture* texture)
{
	diffuse_texture = texture;

	if (mesh != nullptr)
	{
		checkers_image = !mesh->SetTexture(diffuse_texture);
	}
}

bool Material::DeleteTexture()
{
	bool ret = true;

	TextureImporter::UnloadTexture(diffuse_texture->id);
	delete diffuse_texture;
	diffuse_texture = nullptr;

	return ret;
}
