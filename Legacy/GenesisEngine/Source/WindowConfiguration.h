#pragma once
#include "EditorWindow.h"
#include <vector>

typedef int GLint;

class WindowConfiguration : public EditorWindow {
public:
	WindowConfiguration();
	~WindowConfiguration();

	void Draw() override;

private:
	void GetMemoryStatistics(const char* gpu_brand, GLint& vram_budget, GLint& vram_usage, GLint& vram_available, GLint& vram_reserved);

private:
	std::vector<float> fps_log;
	std::vector<float> ms_log;
};