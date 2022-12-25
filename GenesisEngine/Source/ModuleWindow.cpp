#include "Globals.h"
#include "Engine.h"
#include "ModuleWindow.h"

#include "HelperClasses/GnJSON.h"

#include <string>
#include "SDL/include/SDL_video.h"

ModuleWindow::ModuleWindow(GnEngine* app, bool start_enabled) : Module(app, start_enabled),
	width(1280), height(1024), brightness(1.0f),
	fullscreen(false), resizable(true), borderless(false), fullscreenDesktop(false)
{
	name = "window";

	window = NULL;
	screen_surface = NULL;

	context = nullptr;
}

// Destructor
ModuleWindow::~ModuleWindow()
{}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		if (fullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN;}

		if(resizable) {
			flags |= SDL_WINDOW_RESIZABLE;}

		if(borderless == true) {
			flags |= SDL_WINDOW_BORDERLESS;}

		if(fullscreenDesktop == true) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;}

		char windowTitle[64];
		sprintf(windowTitle, "Genesis Engine %d.%d", Engine->versionMajor, Engine->versionMinor);
		window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG_ERROR("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
			context = SDL_GL_CreateContext(window);

			brightness = SDL_GetWindowBrightness(window);
		}
	}

	return ret;
}

bool ModuleWindow::LoadEditorConfig(GnJSONObj& config)
{
	width = config.GetInt("width", width);

	height = config.GetInt("height", height);

	fullscreen = config.GetBool("fullscreen", fullscreen);
	fullscreenDesktop = config.GetBool("fullscreen_desktop", fullscreenDesktop);
	resizable = config.GetBool("resizable", resizable);
	borderless = config.GetBool("borderless", borderless);

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	SDL_GL_DeleteContext(context);

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

float ModuleWindow::GetBrightness() const
{
	return brightness;
}

void ModuleWindow::SetBrightness(float newBrightness)
{
	brightness = newBrightness;
	if (SDL_SetWindowBrightness(window, brightness) != 0)
		LOG_ERROR("Error changing window brightness %s", SDL_GetError());
}

int ModuleWindow::GetWidht() const { return width; }

int ModuleWindow::GetHeight() const { return height; }

void ModuleWindow::GetSize(int& windowWidth, int& windowHeight)
{
	SDL_GetWindowSize(window, &width, &height);

	windowWidth = width;
	windowHeight = height;
}

void ModuleWindow::SetSize(int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	SDL_SetWindowSize(window, width, height);
}

bool ModuleWindow::IsFullscreen() const { return fullscreen; }

void ModuleWindow::SetFullscreen(bool newFullscreen)
{
	fullscreen = newFullscreen;

	if (fullscreen) { 
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); }
	else {
		SDL_SetWindowFullscreen(window, 0); }
}

bool ModuleWindow::IsFullscreenDesktop() const { return fullscreenDesktop; }

void ModuleWindow::SetFullscreenDesktop(bool newFullscreenDesktop)
{
	fullscreenDesktop = newFullscreenDesktop;

	if (fullscreenDesktop) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);}
	else {
		SDL_SetWindowFullscreen(window, 0);}
}

bool ModuleWindow::IsResizable() const { return resizable; }

void ModuleWindow::SetResizable(bool newResizable)
{
	resizable = newResizable;

	if (resizable) {
		SDL_SetWindowResizable(window, SDL_TRUE);}
	else {
		SDL_SetWindowResizable(window, SDL_FALSE);}
}

bool ModuleWindow::IsBorderless() const { return borderless; }

void ModuleWindow::SetBorderless(bool newBorderless)
{
	borderless = newBorderless;
	if (borderless){
		SDL_SetWindowBordered(window, SDL_FALSE);}
	else { 
		SDL_SetWindowBordered(window, SDL_TRUE);}
}
