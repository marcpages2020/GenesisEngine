#include "GnJSON.h"
#include "parson/parson.h"

//GnJSONObject ====================================================

GnJSONObj::GnJSONObj()
{
	_root = json_value_init_object();
	_object = json_value_get_object(_root);
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
	_root = json_value_init_object();
}

GnJSONObj::~GnJSONObj() 
{
	if (_root != nullptr)
	{
//		json_value_free(_root);
	}
}

JSON_Object* GnJSONObj::GetJSONObject()
{
	return _object;
}

JSON_Value* GnJSONObj::GetValue()
{
	return _root;
}

uint GnJSONObj::Save(char** buffer)
{
	uint size = json_serialization_size_pretty(_root);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(_root, *buffer, size);
	return size;
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

void GnJSONObj::AddInt(const char* name, int number)
{
	json_object_set_number(_object, name, number);
}

void GnJSONObj::AddFloat(const char* name, float number)
{
	json_object_set_number(_object, name, number);
}

void GnJSONObj::AddBool(const char* name, bool boolean)
{
	json_object_set_boolean(_object, name, boolean);
}

void GnJSONObj::AddC_Str(const char* name, const char* string)
{
	json_object_set_string(_object, name, string);
}

GnJSONArray GnJSONObj::AddArray(const char* name)
{
	json_object_set_value(_object, name, json_value_init_array());
	return GnJSONArray(json_object_get_array(_object, name));
}

//GnJSONArray =====================================================

GnJSONArray::GnJSONArray() : _array(nullptr) {}

GnJSONArray::GnJSONArray(JSON_Array* g_array)
{
	_array = g_array;
}

GnJSONArray::~GnJSONArray() 
{
	delete _array;
	_array = nullptr;
}

GnJSONObj GnJSONArray::GetObjectInArray(const char* name)
{
	int count = json_array_get_count(_array);

	for (size_t i = 0; i < count; i++)
	{
		JSON_Object* object = json_array_get_object(_array, i);
		const char* object_name = json_object_get_string(object, "name");

		if (strcmp(name, object_name) == 0)
		return GnJSONObj(object);
	}

	LOG_ERROR("JSON object %s could not be found", name);
}

void GnJSONArray::AddObject(GnJSONObj object)
{
	json_array_append_value(_array, object.GetValue());
}
