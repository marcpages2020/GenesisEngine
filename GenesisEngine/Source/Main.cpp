#include <stdlib.h>
#include "Engine.h"
#include "Globals.h"

//SDL
#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
 
enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Engine* engine = NULL;

int main(int argc, char* argv[])
{
	LOG("Starting Genesis Engine...");

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			LOG("-------------- Engine Creation --------------");
			engine = new Engine(argc, argv);
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("-------------- Engine Init --------------");
			if (engine->Init() == false)
			{
				LOG("Engine Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				LOG("-------------- Engine Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = engine->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("Engine Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("-------------- Engine CleanUp --------------");
			if (engine->CleanUp() == false)
			{
				LOG("Engine CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete engine;
	engine = nullptr;
	LOG("Exiting Genesis Engine...\n");
	return main_return;
}