#ifndef _WINDOW_IMPORT_H_
#define _WINDOW_IMPORT_H_
#include "EditorWindow.h"
#include "ImportingOptions.h"
#include "Resource.h"

class WindowImport : public EditorWindow {
public:
	WindowImport();
	~WindowImport();

	void Draw() override;
	bool DrawModelImportingWindow();
	bool DrawTextureImportingWindow();

	void Enable(const char* file, ResourceType resourceType);

private: 
	char _fileToImport[256];
	ModelImportingOptions _modelImportingOptions;
	TextureImportingOptions _textureImportingOptions;
	ResourceType _currentResourceType;
};

#endif
