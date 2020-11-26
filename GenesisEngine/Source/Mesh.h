#pragma once
#include "Globals.h"
#include "Component.h"
#include <vector>;
#include "Material.h"

#include "MathGeoLib/include/MathGeoLib.h"

class ResourceMesh;

typedef float GLfloat;
typedef unsigned short GLushort;
typedef unsigned char GLubyte;

class GnMesh : public Component {
public:
	GnMesh();
	virtual ~GnMesh();

	void Save(GnJSONArray& save_array) override;
	void Load(GnJSONObj& load_object) override;
	void SetResourceUID(uint UID) override;
	Resource* GetResource(ResourceType type) override;

	void GenerateAABB();
	AABB GetAABB();

	virtual void Update() override;
	virtual void Render();
	virtual void OnEditor() override;

	void DrawVertexNormals();
	void DrawFaceNormals();

public:
	const char* name;
	char* path;

private:
	AABB _AABB;

	bool draw_vertex_normals;
	bool draw_face_normals;

	ResourceMesh* _resource;
	
};

class GnCube : public GnMesh {
public:
	GnCube();
	~GnCube();
};

class GnPlane : public GnMesh {
public:
	GnPlane();
	~GnPlane();
};

class GnPyramid : public GnMesh {
public:
	GnPyramid();
	~GnPyramid();
};

class GnSphere : public GnMesh {
public:
	GnSphere();
	~GnSphere();

	void Render() override;

private: 
	std::vector<GLfloat> vertices;
	std::vector<GLushort> indices;
};

class GnCylinder : public GnMesh {
public:
	GnCylinder();
	GnCylinder(float radius, float height, int sides);
	~GnCylinder();
	
	void CalculateGeometry();
private:
	float radius = 0;
	float height = 0;
	unsigned int sides = 0;
};

class GnGrid {
public:
	GnGrid(int size);
	~GnGrid();

	void Render();

private:
	int size;
};

class GnCone : public GnMesh {
public:
	GnCone();
	GnCone(float radius, float height, int sides);
	~GnCone();

	void CalculateGeometry(int sides);

private: 
	float radius;
	float height;
};
