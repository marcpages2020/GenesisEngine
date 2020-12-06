#pragma once

enum Axis
{
	X,
	Y,
	Z,
	MINUS_X,
	MINUS_Y,
	MINUS_Z
};

struct ModelImportingOptions
{
	float globalScale = 1.0f;
	Axis forwardAxis = Axis::Z;
	Axis upAxis = Axis::Y;
	bool normalizeScales = false;
	bool ignoreCameras = true;
	bool ignoreLights = true;
};

enum class TextureWrap
{
	CLAMP_TO_BORDER,
	CLAMP,
	REPEAT,
	MIRRORED_REPEAT
};

enum class TextureFiltering
{
	NEAREST,
	LINEAR
};

struct TextureImportingOptions
{
	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;
	bool flip = false;
};