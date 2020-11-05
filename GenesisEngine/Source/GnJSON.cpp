#include "GnJSON.h"
#include "parson/parson.h"

//GnJSONObject ====================================================

GnJSONObj::GnJSONObj() : _object(nullptr), _root(nullptr) {}

GnJSONObj::GnJSONObj(char* buffer)
{
	_root = json_parse_string(buffer);
	_object = json_value_get_object(_root);
	//JSON_Array* modules = json_object_get_array(_object, "modules");

	if (_root != NULL)
	{
		LOG("Config file loaded successfully");
	}
	else
	{
		LOG_ERROR("Error trying to load config file");
	}
}

GnJSONObj::GnJSONObj(JSON_Object* object)
{
	_object = object;
}

GnJSONObj::~GnJSONObj() 
{
	delete _object;
	_object = nullptr;

	if(_root != nullptr)
	 json_value_free(_root);
}

GnJSONArray* GnJSONObj::GetArray(const char* name)
{
	GnJSONArray* array = new GnJSONArray(json_object_get_array(_object, name));
	return array;
}

//GnJSONArray =====================================================

GnJSONArray::GnJSONArray() : array(nullptr) {}

GnJSONArray::GnJSONArray(JSON_Array* g_array)
{
	array = g_array;
}

GnJSONArray::~GnJSONArray() 
{
	delete array;
	array = nullptr;
}

GnJSONObj* GnJSONArray::GetObjectInArray(const char* name)
{
	for (size_t i = 0; i < json_array_get_count(array); i++)
	{
		JSON_Object* object = json_array_get_object(array, i);
		if (strcmp(name, json_object_get_string(object, "name")) == 0)
			return new GnJSONObj(object);
	}

	LOG_ERROR("JSON object %s could not be found", name);
	return NULL;
}
