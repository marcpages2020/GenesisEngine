#ifndef _TEXTURE_IMPORTER_H_
#define _TEXTURE_IMPORTER_H_
#include "Globals.h"

struct TextureImportingOptions;
class ResourceTexture;

typedef unsigned int ILenum;

namespace TextureImporter
{
	void Init();

	void Import(char* fileBuffer, ResourceTexture* resource, uint size);
	uint Save(ResourceTexture* texture, char** fileBuffer);
	bool Load(char* fileBuffer, ResourceTexture* texture, uint size);

	void UnloadTexture(uint imageID);
	ILenum GetFileFormat(const char* file);
	void ApplyImportingOptions(TextureImportingOptions importingOptions);
}

#endif // !_TEXTURE_IMPORTER_H_

