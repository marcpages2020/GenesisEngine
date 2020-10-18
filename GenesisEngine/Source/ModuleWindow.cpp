#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "parson/parson.h"

#include "SDL/include/SDL_video.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	name = "window";
	window = NULL;
	screen_surface = NULL;

	context = nullptr;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
	window = NULL;
	screen_surface = nullptr;
	context = nullptr;
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		//int width = SCREEN_WIDTH * SCREEN_SIZE;
		//int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if (fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		static char title[2048] = { 0 };
		memset(title, 0, sizeof(title));
		sprintf_s(title, "%s v%s", App->engine_name, App->GetEngineVersion());

		window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if (window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			context = SDL_GL_CreateContext(window);
		}

		brightness = SDL_GetWindowBrightness(window);
	}

	return ret;
}

bool ModuleWindow::LoadConfig(JSON_Object* config)
{
	//Get Json attributes
	width = json_object_get_number(config, "width");
	height = json_object_get_number(config, "height");

	fullscreen = json_object_get_boolean(config, "fullscreen");
	fullscreen_desktop = json_object_get_boolean(config, "fullscreen_desktop");
	resizable = json_object_get_boolean(config, "resizable");
	borderless = json_object_get_boolean(config, "borderless");

	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	SDL_GL_DeleteContext(context);

	//Destroy window
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::GetSize(int& g_width, int& g_height)
{
	SDL_GetWindowSize(window, &g_width, &g_height);
}

void ModuleWindow::SetSize(int g_width, int g_height)
{
	SDL_SetWindowSize(window, g_width, g_height);
}

float ModuleWindow::GetBrightness()
{
	return brightness;
}

void ModuleWindow::SetBrightness(float g_brightness)
{
	brightness = g_brightness;
	if (SDL_SetWindowBrightness(window, brightness) != 0)
		LOG("Error changing window brightness %s", SDL_GetError());
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetFullscreen(bool setFullscreen)
{
	if (setFullscreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetFullscreenDesktop(bool setFullscreenDesktop)
{
	if (setFullscreenDesktop)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
}

void ModuleWindow::SetResizable(bool setResizable)
{
	if (setResizable)
		SDL_SetWindowResizable(window, SDL_TRUE);
	else
		SDL_SetWindowResizable(window, SDL_FALSE);
}

void ModuleWindow::SetBorderless(bool setBorderless)
{
	if (setBorderless)
		SDL_SetWindowBordered(window, SDL_FALSE);
	else
		SDL_SetWindowBordered(window, SDL_TRUE);
}

void ModuleWindow::OnResize(int g_width, int g_height)
{
	width = g_width;
	height = g_height;
}
