#include "TextureImporter.h"
#include "Application.h"
#include "FileSystem.h"
#include "Timer.h"
#include "ResourceTexture.h"
#include "ImportingOptions.h"

#include "Devil/include/IL/il.h"
#include "Devil/include/IL/ilu.h"
#include "Devil/include/IL/ilut.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")	
#pragma comment (lib, "Devil/libx86/ILU.lib")	
#pragma comment (lib, "Devil/libx86/ILUT.lib")	

void TextureImporter::Init()
{
	ilInit();
	iluInit();

	if (ilutRenderer(ILUT_OPENGL))
		LOG("DevIL initted correctly");
}

void TextureImporter::Import(char* fileBuffer, ResourceTexture* texture, uint size)
{
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error = IL_NO_ERROR;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	//ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		//LOG_ERROR("%s, %d %s", texture->assetsFile.c_str(), ilGetError(), iluErrorString(error));
		//return;
	}

	ILenum file_format = GetFileFormat(texture->assetsFile.c_str());

	if (ilLoadL(file_format, fileBuffer, size) == IL_FALSE)
	{
		LOG_ERROR("Error importing texture %s - %d: %s", texture->assetsFile.c_str(), ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		LOG_ERROR("%d: %s", error, iluErrorString(error));
	}
	else
	{
		LOG("Texture loaded successfully from: %s in %d ms", texture->assetsFile.c_str(), timer.Read());

		texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}

	ilBindImage(0);
}

uint TextureImporter::Save(ResourceTexture* texture, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;

	ilBindImage(texture->GetID());

	TextureImportingOptions importingOptions = App->resources->textureImportingOptions;
	ApplyImportingOptions(App->resources->textureImportingOptions);

	ilSetInteger(IL_DXTC_DATA_FORMAT, IL_DXT5);
	size = ilSaveL(IL_DDS, nullptr, 0);

	if (size > 0)
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
			*fileBuffer = (char*)data;
	}
	else
	{
		ILenum error;
		error = ilGetError();

		if (error != IL_NO_ERROR)
		{
			LOG_ERROR("Error when saving %s - %d: %s", texture->assetsFile, error, iluErrorString(error));
		}
	}

	ilBindImage(0);

	return size;
}

bool TextureImporter::Load(char* fileBuffer, ResourceTexture* texture, uint size)
{
	bool ret = true;
	Timer timer;
	timer.Start();

	ILuint imageID = 0;
	ILenum error;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	if (ilLoadL(IL_DDS, fileBuffer, size) == IL_FALSE)
	{
		LOG_ERROR("Error loading texture %s - %d: %s", texture->libraryFile.c_str(), ilGetError(), iluErrorString(ilGetError()));

		ilBindImage(0);
		ilDeleteImages(1, &imageID);
		return false;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	error = ilGetError();
	if (error != IL_NO_ERROR)
	{
		//ilBindImage(0);
		//ilDeleteImages(1, &imageID);
		//LOG_ERROR("Error %d when loading %s: %s", error, texture->libraryFile.c_str(), iluErrorString(error));
		//ret = false;
	}
	else
	{
		//LOG("Texture loaded successfully from: %s in %d ms", texture->libraryFile.c_str(), timer.Read());
		//texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}

	LOG("Texture loaded successfully from: %s in %d ms", texture->libraryFile.c_str(), timer.Read());
	texture->FillData(ilGetData(), (uint)imageID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));

	ilBindImage(0);
	return ret;
}

void TextureImporter::UnloadTexture(uint imageID)
{
	ilDeleteImages(1, &imageID);
}

ILenum TextureImporter::GetFileFormat(const char* file)
{
	ILenum file_format = IL_TYPE_UNKNOWN;
	std::string format = FileSystem::GetFileExtension(file);

	if (format == ".png")
		file_format = IL_PNG;
	else if (format == ".jpg")
		file_format = IL_JPG;
	else if (format == ".bmp")
		file_format = IL_BMP;

	return file_format;
}

void TextureImporter::ApplyImportingOptions(TextureImportingOptions importingOptions)
{
	if (importingOptions.flip) {
		if (iluFlipImage() == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully flipped");
	}

	if (importingOptions.alienify) {
		if(iluAlienify() == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully alienified");
	}

	if (importingOptions.blur_average) {
		if(iluBlurAvg(importingOptions.blur_average_iterations) == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully averagly blurred");
	}

	if (importingOptions.blur_gaussian) {
		if(iluBlurGaussian(importingOptions.blur_gaussian_iterations) == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully gaussiany blurred");
	}

	if (importingOptions.equalize) {
		if(iluEqualize() == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully equalized");
	}


	if (importingOptions.negativity) {
		if(iluNegative() == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully negativized");
	}


	if (importingOptions.noise) {
		if(iluNoisify(importingOptions.noise_tolerance) == IL_FALSE)
			LOG_ERROR("Couldn't add noise to the image");
	}

	if (importingOptions.pixelize) {
		if(iluPixelize(importingOptions.pixelize_size) == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully pixelized");
	}

	if (importingOptions.sharpening) {
		if(iluSharpen(importingOptions.sharpening_factor, importingOptions.sharpening_iterations) == IL_FALSE)
			LOG_ERROR("The image couldn't be successfully sharpened");
	}
	
	if(iluContrast(importingOptions.contrast) == IL_FALSE)
		LOG("Couldn't set image's contrast to %.2f", importingOptions.contrast);

	if (iluGammaCorrect(importingOptions.gamma_correction) == IL_FALSE)
		LOG("Couldn't correct image's gamma");

	//iluImageParameter(ILU_FILTER, );

	float contrast = 1.0f;
}