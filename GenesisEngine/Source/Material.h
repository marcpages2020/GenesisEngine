#pragma once
#include "Component.h"

class Material : public Component {
public: 
	Material();
	~Material();
	void OnEditor() override;
};