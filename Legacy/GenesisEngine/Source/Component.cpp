#include "Component.h"

Component::Component() : enabled(true), _gameObject(nullptr) {}

Component::Component(GameObject* gameObject) 
{
    _gameObject = gameObject;
}

Component::~Component()
{
    _gameObject = nullptr;
}

void Component::Update(){}

void Component::Enable() { enabled = true; }

void Component::Disable() { enabled = false; }

bool Component::IsEnabled()
{
    return enabled;
}

ComponentType Component::GetType()
{
    return type;
}

void Component::SetGameObject(GameObject* g_gameObject)
{
    _gameObject = g_gameObject;
}

GameObject* Component::GetGameObject()
{
    return _gameObject;
}

void Component::SetResourceUID(uint UID)
{
    _resourceUID = UID;
}
