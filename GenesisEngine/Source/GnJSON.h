#pragma once

#include "Globals.h"

struct json_array_t;
typedef json_array_t JSON_Array;

struct json_value_t;
typedef json_value_t JSON_Value;

struct json_object_t;
typedef json_object_t JSON_Object;

class GnJSONArray;

class GnJSONObj 
{
public:
	GnJSONObj();
	GnJSONObj(char* buffer);
	GnJSONObj(JSON_Object* object);

	~GnJSONObj();

	GnJSONArray* GetArray(const char* name);

private:
	JSON_Object* _object;
	JSON_Value* _root;
};

class GnJSONArray 
{
public:
	GnJSONArray();
	GnJSONArray(JSON_Array* array);
	~GnJSONArray();

	GnJSONObj* GetObjectInArray(const char* name);

private: 
	JSON_Array* array;
};
