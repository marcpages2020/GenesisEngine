#include "WindowImport.h"
#include "Application.h"
#include "ImGui/imgui.h"

WindowImport::WindowImport() : EditorWindow(), _fileToImport(nullptr), _currentResourceType(ResourceType::RESOURCE_UNKNOWN)
{}

WindowImport::~WindowImport()
{
	_fileToImport = nullptr;
}

void WindowImport::Draw()
{
	if (_currentResourceType == ResourceType::RESOURCE_MODEL) 
	{
		visible = DrawModelImportingWindow();
	}
	else if (_currentResourceType == ResourceType::RESOURCE_TEXTURE) 
	{
		visible = DrawTextureImportingWindow();
	}
	else
	{
		LOG_ERROR("Trying to import invalid file %s", _fileToImport);
	}
}

void WindowImport::Enable(const char* file, ResourceType resourceType)
{
	_fileToImport = file;
	_currentResourceType = resourceType;
	visible = true;
}

bool WindowImport::DrawModelImportingWindow()
{
	bool ret = true;

	if (ImGui::Begin("Import File", NULL))
	{
		ImGui::Text("Import Model: %s", _fileToImport);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::DragFloat("Global Scale", &_modelImportingOptions.globalScale, 0.01f, 0.0f, 100.0f);

		const char* possible_axis[] = { "X", "Y", "Z", "-X", "-Y", "-Z" };
		int forward_axis = (int)_modelImportingOptions.forwardAxis;
		if (ImGui::Combo("Forward Axis", &forward_axis, possible_axis, 6))
			_modelImportingOptions.forwardAxis = (Axis)forward_axis;

		int up_axis = (int)_modelImportingOptions.upAxis;
		if (ImGui::Combo("Up Axis", &up_axis, possible_axis, 6))
			_modelImportingOptions.upAxis = (Axis)up_axis;

		ImGui::Checkbox("Ignore Cameras", &_modelImportingOptions.ignoreCameras);

		ImGui::Spacing();

		ImGui::Checkbox("Ignore Lights", &_modelImportingOptions.ignoreLights);

		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->modelImportingOptions = _modelImportingOptions;
			App->resources->ImportFile(_fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20))) 
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			_modelImportingOptions = ModelImportingOptions();
	}
	ImGui::End();

	return ret;
}

bool WindowImport::DrawTextureImportingWindow()
{
	bool ret = true;

	if (ImGui::Begin("Import File", NULL))
	{
		ImGui::Text("Import Texture: %s", _fileToImport);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		const char* texture_wrap_options[] = { "Clamp To Border", "Clamp", "Repeat", "Mirrored Repeat" };
		int texture_wrap = (int)_textureImportingOptions.textureWrap;
		if (ImGui::Combo("Texture Wrap", &texture_wrap, texture_wrap_options, 4))
			_textureImportingOptions.textureWrap = (TextureWrap)texture_wrap;

		const char* texture_filtering_options[] = { "Nearest", "Linear" };
		int texture_filtering = (int)_textureImportingOptions.textureFiltering;
		if (ImGui::Combo("Texture Filtering", &texture_filtering, texture_filtering_options, 2))
			_textureImportingOptions.textureFiltering = (TextureFiltering)texture_filtering;

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Columns(3);
		ImGui::Checkbox("Flip", &_textureImportingOptions.flip);
		//ImGui::SameLine();
		ImGui::Checkbox("Alienify", &_textureImportingOptions.alienify);
		//ImGui::SameLine();
		ImGui::Checkbox("Equalize", &_textureImportingOptions.equalize);

		ImGui::NextColumn();
		ImGui::Checkbox("Blur Average", &_textureImportingOptions.blur_average);
		ImGui::Checkbox("Blur Gaussian", &_textureImportingOptions.blur_gaussian);
		//ImGui::SameLine();
		ImGui::Checkbox("Negativity", &_textureImportingOptions.negativity);

		ImGui::NextColumn();
		ImGui::Checkbox("Noise", &_textureImportingOptions.noise);
		//ImGui::SameLine();
		ImGui::Checkbox("Pixelize", &_textureImportingOptions.pixelize);
		//ImGui::SameLine();
		ImGui::Checkbox("Sharpening", &_textureImportingOptions.sharpening);

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Columns(1);
		ImGui::SliderFloat("Contrast", &_textureImportingOptions.contrast, 0.0f, 1.7f);
		ImGui::SliderFloat("Gamma Correction", &_textureImportingOptions.gamma_correction, 0.0f, 2.0f);
		ImGui::SliderFloat("Noise Tolerance", &_textureImportingOptions.noise_tolerance, 0.0f, 1.0f);
		ImGui::SliderInt("Pixel Size", &_textureImportingOptions.pixelize_size, 0, 20);
		ImGui::SliderFloat("Sharpening factor", &_textureImportingOptions.sharpening_factor, 0.0f, 2.5f);
		ImGui::SliderInt("Sharpening Iterations", &_textureImportingOptions.sharpening_iterations, 1, 10);


		ImGui::Spacing();
		if (ImGui::Button("OK", ImVec2(40, 20))) 
		{
			App->resources->textureImportingOptions = _textureImportingOptions;
			App->resources->ImportFile(_fileToImport);
			ret = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("CANCEL", ImVec2(80, 20)))
			ret = false;
		ImGui::SameLine();
		if (ImGui::Button("Reset"))
			_textureImportingOptions = TextureImportingOptions();
	}
	ImGui::End();

	return ret;
}
