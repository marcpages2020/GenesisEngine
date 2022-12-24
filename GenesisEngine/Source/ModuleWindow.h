#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class GnEngine;

class ModuleWindow : public Module
{
public:

	ModuleWindow(GnEngine* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init() override;
	bool CleanUp();

	void SetTitle(const char* title);

	float GetBrightness() const;
	void SetBrightness(float newBrightness);

	int GetWidht() const;
	int GetHeight() const;
	void GetSize(int& windowWidth, int& windowHeight);
	void SetSize(int newWidth, int newHeight);

	bool IsFullscreen() const;
	void SetFullscreen(bool newFullscreen);
	bool IsFullscreenDesktop() const;
	void SetFullscreenDesktop(bool newFullscreenDesktop);
	bool IsResizable() const;
	void SetResizable(bool newResizable);
	bool IsBorderless() const;
	void SetBorderless(bool newBorderless);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
	SDL_GLContext context;

private: 
	float brightness;
	int width;
	int height;

	bool fullscreen;
	bool fullscreenDesktop;
	bool resizable;
	bool borderless;
};

#endif // __ModuleWindow_H__