#include "ResourceTexture.h"
#include "GnJSON.h"
#include "glew/include/glew.h"
#include "Application.h"

ResourceTexture::ResourceTexture(uint UID) : Resource(UID, ResourceType::RESOURCE_TEXTURE),
_id(0), _width(-1), _height(-1), _data(nullptr) , gpu_ID(-1){}

ResourceTexture::~ResourceTexture() {}

void ResourceTexture::GenerateBuffers()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gpu_ID);
	glBindTexture(GL_TEXTURE_2D, gpu_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourceTexture::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, gpu_ID);
}

void ResourceTexture::FillData(GLubyte* data, uint id, int width, int height)
{
	_data = data;
	_id = id;
	_width = width;
	_height = height;
}

uint ResourceTexture::SaveMeta(GnJSONObj& base_object, uint last_modification)
{
	base_object.AddInt("UID", _uid);
	base_object.AddInt("lastModified", last_modification);
	base_object.AddString("Library path", libraryFile.c_str());

	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;
	
	base_object.AddInt("textureWrap", (int)App->resources->textureImportingOptions.textureWrap);
	base_object.AddInt("textureFiltering", (int)App->resources->textureImportingOptions.textureFiltering);
	//base_object.AddInt("width", width);
	//base_object.AddInt("height", height);

	return 1;
}

void ResourceTexture::Load(GnJSONObj& base_object)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &gpu_ID);
	glBindTexture(GL_TEXTURE_2D, gpu_ID);

	TextureWrap textureWrap = (TextureWrap)base_object.GetInt("textureWrap", 2);

	switch (textureWrap)
	{
	case TextureWrap::CLAMP_TO_BORDER:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		break;
	case TextureWrap::CLAMP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;
	case TextureWrap::REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case TextureWrap::MIRRORED_REPEAT:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	default:
		break;
	}

	TextureFiltering textureFiltering = (TextureFiltering)base_object.GetInt("textureFiltering", 0);

	if (textureFiltering == TextureFiltering::NEAREST) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
	glBindTexture(GL_TEXTURE_2D, 0);

}

uint ResourceTexture::GetID() {	return _id; }

int ResourceTexture::GeWidth() { return _width; }

int ResourceTexture::GetHeight() {	return _height; }

GLubyte* ResourceTexture::GetData() { return _data; }

