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
	GnJSONObj(const char* buffer);
	GnJSONObj(JSON_Object* object);
	~GnJSONObj();

	JSON_Object* GetJSONObject();
	JSON_Value* GetValue();

	uint Save(char** buffer);

	JSON_Array* GetParsonArray(const char* name);

	int GetInt(const char* name);
	float GetFloat(const char* name);
	bool GetBool(const char* name);
	const char* GetC_Str(const char* name);

	void AddInt(const char* name, int number);
	void AddFloat(const char* name, float number);
	void AddBool(const char* name, bool boolean);
	void AddC_Str(const char* name, const char* string);
	GnJSONArray AddArray(const char* name);

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

	GnJSONObj GetObjectInArray(const char* name);

	void AddObject(GnJSONObj object);

private: 
	JSON_Array* _array;
};
