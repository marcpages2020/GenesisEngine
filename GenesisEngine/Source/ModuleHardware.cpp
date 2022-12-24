#include "Globals.h";
#include "ModuleHardware.h"
#include <iostream>
#include "SDL/include/SDL.h"
#include "glew/include/glew.h"

ModuleHardware::ModuleHardware(GnEngine* app, bool start_enabled) : Module(app, start_enabled),
CPUCount(-1), CPUCache(-1), RAM(0), GPU(""), GPUBrand(""),
VRAMBudget(-1), VRAMUsage(-1), VRAMAvailable(-1), VRAMReserved(-1),
RDTSC(false), MMX(false), SSE(false), SSE2(false), SSE3(false), SSE41(false), SSE42(false), 
AVX(false), AVX2(false), AltiVec(false)
{
    name = "hardware";
}

bool ModuleHardware::Start()
{
    //CPU
    CPUCount = SDL_GetCPUCount();
    CPUCache = SDL_GetCPUCacheLineSize();

    //RAM
    RAM = SDL_GetSystemRAM();

    //Caps
    RDTSC   = SDL_HasRDTSC();
    MMX     = SDL_HasMMX();
    SSE     = SDL_HasSSE();
    SSE2    = SDL_HasSSE2();
    SSE3    = SDL_HasSSE3();
    SSE41   = SDL_HasSSE41();
    SSE42   = SDL_HasSSE42();
    AVX     = SDL_HasAVX();
    AVX2    = SDL_HasAVX2();
    AltiVec = SDL_HasAltiVec();

    GPU = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    GPUBrand = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    return true;
}

//CPU
int ModuleHardware::GetCPUCount() const { return CPUCount; }

int ModuleHardware::GetCPUCache() const { return CPUCache; }

//RAM
int ModuleHardware::GetSystemRAM() { return RAM; }

//CAPS
bool ModuleHardware::HasRDTSC() { return RDTSC; }

bool ModuleHardware::HasMMX() { return MMX; }

bool ModuleHardware::HasSSE() { return SSE; }

bool ModuleHardware::HasSSE2() { return SSE2; }

bool ModuleHardware::HasSSE3() { return SSE3; }

bool ModuleHardware::HasSSE41() { return SSE41; }

bool ModuleHardware::HasSSE42() { return SSE42; }

bool ModuleHardware::HasAVX() { return AVX; }

bool ModuleHardware::HasAVX2() { return AVX2; }

bool ModuleHardware::HasAltiVec() { return AltiVec; }

void ModuleHardware::GetAllCaps(char* char_caps)
{
    std::string caps;
    if (RDTSC) { caps += "RDTSC, "; }
    if (MMX) { caps += "MMX, "; }
    if (SSE) { caps += "SSE, "; }
    if (SSE2) { caps += "SSE2, "; }
    if (SSE3) { caps += "SSE3, "; }
    if (SSE41) { caps += "SSE41, "; }
    if (SSE42) { caps += "SSE42, "; }
    if (AVX) { caps += "AVX, "; }
    if (AVX2) { caps += "AVX2, "; }
    if (AltiVec) { caps += "AltiVec"; }

    strcpy(char_caps, caps.c_str());
}

const char* ModuleHardware::GetGPU() { return GPU; }

const char* ModuleHardware::GetGPUBrand() { return GPUBrand; }

//Get VRAM Budget in Mb
int ModuleHardware::GetVRAMBudget() 
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &VRAMBudget);
    VRAMBudget /= 1000.0f;
    return VRAMBudget; 
}

//Get VRAM Usage in Mb
int ModuleHardware::GetVRAMUsage()  
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &VRAMUsage);
    VRAMUsage /= 1000.0f;
    return VRAMUsage; 
}

//Get VRAM Available in Mb
int ModuleHardware::GetVRAMAvailable()  
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &VRAMAvailable);
    VRAMAvailable /= 1000.0f;
    return VRAMAvailable; 
}

//Get VRAM Reserved in Mb
int ModuleHardware::GetVRAMReserved() 
{
    glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &VRAMReserved);
    VRAMReserved /= 1000.0f;
    return VRAMReserved; 
}
