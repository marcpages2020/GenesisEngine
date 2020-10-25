#include "Material.h"
#include "Mesh.h"
#include "ImGui/imgui.h"

Material::Material(): mesh(nullptr), checkers_image(true) {}

Material::Material(GnMesh* g_mesh, GnTexture g_diffuse_texture) : Component(), checkers_image(true)
{
	mesh = g_mesh;

	if (g_diffuse_texture.data != nullptr)
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
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enabled", &enabled);
		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers Image", &checkers_image))
		{
			if (checkers_image == false)
				mesh->SetTexture(diffuse_texture);
			else
				mesh->AssignCheckersImage();
		}

		ImGui::Separator();

		ImGui::Text("Diffuse Texture: %s", diffuse_texture.name.c_str());
		ImGui::Text("Path: %s", diffuse_texture.path.c_str());

		ImGui::Text("Width: %d Height: %d", diffuse_texture.width, diffuse_texture.height);

		ImGui::Spacing();
		ImGui::Image((ImTextureID)diffuse_texture.id, ImVec2(100,100), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::Spacing();
	}
}

void Material::SetTexture(GnTexture texture)
{
	diffuse_texture = texture;

	if (mesh != nullptr)
	{
		mesh->SetTexture(diffuse_texture);
	}
}
