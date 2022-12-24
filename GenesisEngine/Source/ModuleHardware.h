#pragma once
#include "Module.h"

class ModuleHardware : public Module
{
public:
	ModuleHardware(GnEngine* app, bool start_enabled = true);

	bool Start() override;

	//CPU
	int GetCPUCount() const;
	int GetCPUCache() const;

	//RAM in MB
	int GetSystemRAM();

	//Caps
	bool HasRDTSC();
	bool HasMMX();
	bool HasSSE();
	bool HasSSE2();
	bool HasSSE3();
	bool HasSSE41();
	bool HasSSE42();
	bool HasAVX();
	bool HasAVX2();
	bool HasAltiVec();

	void GetAllCaps(char* char_caps);

	const char* GetGPU();
	const char* GetGPUBrand();

	int GetVRAMBudget();
	int GetVRAMUsage();
	int GetVRAMAvailable();
	int GetVRAMReserved();

private:

	//CPU
	int CPUCount;
	int CPUCache;
	
	//RAM
	int RAM;

	//CAPS
	bool RDTSC;
	bool MMX;
	bool SSE;
	bool SSE2;
	bool SSE3;
	bool SSE41;
	bool SSE42;
	bool AVX;
	bool AVX2;
	bool AltiVec;

	const char* GPU;
	const char* GPUBrand;

	int VRAMBudget;
	int VRAMUsage;
	int VRAMAvailable;
	int VRAMReserved;
};

