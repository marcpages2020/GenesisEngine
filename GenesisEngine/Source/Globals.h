#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(LogType::LOG_NORMAL,__FILE__, __LINE__, format, __VA_ARGS__);
#define LOG_WARNING(format, ...) log(LogType::LOG_WARNING,__FILE__, __LINE__, format, __VA_ARGS__);
#define LOG_ERROR(format, ...) log(LogType::LOG_ERROR,__FILE__, __LINE__, format, __VA_ARGS__);

enum class LogType
{
	LOG_NORMAL,
	LOG_WARNING,
	LOG_ERROR
};

void log(LogType logType, const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI

const float M_PI = 3.14159265358979323846f;

typedef unsigned int uint;
typedef unsigned __int64 uint64;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

#define ARRAY_COUNT(array) (sizeof(array)/sizeof(array[0]))

#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Configuration -----------
#define SCREEN_SIZE 1