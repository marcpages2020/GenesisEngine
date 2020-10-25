#include "Material.h"
#include "Mesh.h"
#include "ImGui/imgui.h"

Material::Material()
{

}

Material::Material(GnMesh* g_mesh, GnTexture g_diffuse_texture)
{
	mesh = g_mesh;

	if (g_diffuse_texture.data != nullptr)
	{
		diffuse_texture = g_diffuse_texture;
		mesh->SetTexture(diffuse_texture);
	}
}

Material::~Material()
{
}

void Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Checkbox("Enabled", &enabled))
			enabled = !enabled;

		ImGui::Separator();

		ImGui::Text("Diffuse Texture: %s", diffuse_texture.name.c_str());
		ImGui::Text("Path: %s", diffuse_texture.path.c_str());

		ImGui::Text("Width: %d Height: %d", diffuse_texture.width, diffuse_texture.height);
		ImGui::Spacing();
	}
}

void Material::SetTexture(GnTexture texture)
{
	diffuse_texture = texture;
}
