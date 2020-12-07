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
	bool normalizeScales = true;
	bool ignoreCameras = false;
	bool ignoreLights = false;
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
	bool alienify = false;
	bool blur_average = false;
	bool blur_gaussian = false;
	bool equalize = false;
	bool negativity = false;
	bool noise = false;
	bool pixelize = false;
	int pixelize_size = 1;
	bool sharpening = false;
	float sharpening_factor = 1.0f;
	int sharpening_iterations = 1;
	float noise_tolerance = 0.5f;
	float gamma_correction = 1.0f;
	float contrast = 1.0f;
};