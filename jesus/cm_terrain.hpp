#pragma once

#include "pch.hpp"

struct cm_triangle
{
	fvec3 a;
	fvec3 b;
	fvec3 c;
	vec4_t plane;
	bool edge_walkable = true;

	fvec3 get_mins() const noexcept {
		fvec3 lowest = FLT_MAX;

		lowest.x = a.x;
		if (b.x < lowest.x) lowest.x = b.x;
		if (c.x < lowest.x) lowest.x = c.x;

		lowest.y = a.y;
		if (b.y < lowest.y) lowest.y = b.y;
		if (c.y < lowest.y) lowest.y = c.y;

		lowest.z = a.z;
		if (b.z < lowest.z) lowest.z = b.z;
		if (c.z < lowest.z) lowest.z = c.z;

		return lowest;

	}
	fvec3 get_maxs() const noexcept {
		fvec3 highest = -FLT_MAX;

		highest.x = a.x;
		if (b.x > highest.x) highest.x = b.x;
		if (c.x > highest.x) highest.x = c.x;

		highest.y = a.y;
		if (b.y > highest.y) highest.y = b.y;
		if (c.y > highest.y) highest.y = c.y;

		highest.z = a.z;
		if (b.z > highest.z) highest.z = b.z;
		if (c.z > highest.z) highest.z = c.z;

		return highest;

	}
};

struct cm_terrain
{
	CollisionAabbTree* aabb = 0;
	cLeaf_t* leaf = 0;
	std::list<cm_triangle> tris;
};

void CM_GetTerrainTriangles(cLeaf_t* leaf, const std::string& material_filter);
void CM_AdvanceAabbTree(CollisionAabbTree* aabbTree, cm_terrain* terrain);

void CM_ShowTerrain(cm_terrain* terrain, struct cplane_s* frustumPlanes);
bool CM_TriangleInView(const cm_triangle* leaf, struct cplane_s* frustumPlanes, int numPlanes);
char CM_IsEdgeWalkable(int edgeIndex, int triIndex);

inline std::list<cm_terrain> cm_terrainpoints;