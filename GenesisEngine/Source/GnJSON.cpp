#include "GnJSON.h"
#include "parson/parson.h"

//GnJSONObject ====================================================

GnJSONObj::GnJSONObj()
{
	_root = nullptr;
	_object = nullptr;
}

GnJSONObj::GnJSONObj(const char* buffer) : _object(nullptr)
{
	_root = json_parse_string(buffer);

	if (_root != NULL)
	{
		_object = json_value_get_object(_root);
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
	_root = nullptr;
}

GnJSONObj::~GnJSONObj() 
{
	if (_root != nullptr)
	{
//		json_value_free(_root);
	}
}

JSON_Array* GnJSONObj::GetParsonArray(const char* name)
{
	return json_object_get_array(_object, name);
}

int GnJSONObj::GetInt(const char* name)
{
	return json_object_get_number(_object, name);
}

float GnJSONObj::GetFloat(const char* name)
{
	return json_object_get_number(_object, name);
}

bool GnJSONObj::GetBool(const char* name)
{
	return json_object_get_boolean(_object, name);
}

const char* GnJSONObj::GetC_Str(const char* name)
{
	return json_object_get_string(_object, name);
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

GnJSONObj GnJSONArray::GetObjectInArray(const char* name)
{
	int count = json_array_get_count(array);

	for (size_t i = 0; i < count; i++)
	{
		JSON_Object* object = json_array_get_object(array, i);
		const char* object_name = json_object_get_string(object, "name");

		if (strcmp(name, object_name) == 0)
		return GnJSONObj(object);
	}

	LOG_ERROR("JSON object %s could not be found", name);
}
