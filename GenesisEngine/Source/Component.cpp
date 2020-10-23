#include "Component.h"

Component::Component() : enabled(true) {}

Component::~Component(){}

void Component::Update(){}

void Component::Enable() { enabled = true; }

void Component::Disable() { enabled = false; }
