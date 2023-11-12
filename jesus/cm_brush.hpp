#pragma once
#include "pch.hpp"

#define MAX_POINTS_ON_WINDING   64

#define SIDE_FRONT  0
#define SIDE_BACK   1
#define SIDE_ON     2
#define SIDE_CROSS  3

#define CLIP_EPSILON    0.1f

#define MAX_MAP_BOUNDS  65535

// you can define on_epsilon in the makefile as tighter
#ifndef ON_EPSILON
#define ON_EPSILON  0.1f
#endif

void Cmd_ShowBrushes_f(char* arg);
void GetBrushPolys(cbrush_t* brush, float(*outPlanes)[4]);

struct sc_winding_t
{
	std::vector<fvec3> points;
	bool is_bounce = false;
};

std::optional<sc_winding_t> CM_GetBrushWinding(cbrush_t* b, const fvec3& normals);

struct showcol_brush
{
	std::vector<sc_winding_t> windings;
	cbrush_t* brush;
	std::vector<SimplePlaneIntersection> intersections;

	std::vector<fvec3> to_triangles() const noexcept {
		std::vector<fvec3> tris;

		size_t numPoints = 0;
		for (auto& winding : windings) {

			numPoints = winding.points.size();
			for (size_t p = 0; p < numPoints; p++) {
				tris.push_back(winding.points[p]);

			}

		}
		return tris;
	}
};
cbrush_t* CM_FindBrushByOrigin(const fvec3& origin);

void CM_BuildAxialPlanes(float(*planes)[6][4], const cbrush_t* brush);
void CM_GetWindingForBrushFace(unsigned int brushSide, cbrush_t* brush, Poly* polygon, const float(*planes)[4], signed int maxVerts);
int CM_BuildWindingsForBrush(cbrush_t* brush, std::vector<Poly>& outPolys, std::vector<float>& maxVerts);
void Phys_GetWindingForBrushFace2(unsigned int brushSideIndex, cbrush_t* brush, Poly* outPolys, signed int maxPolys, float(*axialPlanes)[6][4]);
int Phys_BuildWindingsForBrush(cbrush_t* brush, const float(*planes)[4], Poly* outPolys, float(*outVerts)[3]);
void CM_ReverseWinding(winding_t* w);
bool PlaneFromPoints(vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c);
int GetPointListAllowDupes(SimplePlaneIntersection* pts, int planeIndex, int pointCount, SimplePlaneIntersection** xyz);
int ReduceToACycle(int basePlane, SimplePlaneIntersection** xyz, int ptsCount);
int GetPlaneIntersections(const float** planes, int planeCount, SimplePlaneIntersection* OutPts);
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], int index, float* expandedPlane);
int BrushToPlanes(const cbrush_t* brush, float(*outPlanes)[4]);
adjacencyWinding_t* BuildBrushdAdjacencyWindingForSide(int ptCount, SimplePlaneIntersection* pts, float* sideNormal, int planeIndex, adjacencyWinding_t* optionalOutWinding);

vec_t   WindingArea(winding_t* w);
void    CheckWinding(winding_t* w);
void    WindingPlane(winding_t* w, vec3_t normal, vec_t* dist);

void stealerino_test();

void RB_ShowCollision(GfxViewParms* viewParms);
void RB_RenderWinding(const showcol_brush& sb);
bool CM_BrushInView(const cbrush_t* brush, struct cplane_s* frustumPlanes, int numPlanes);

//have to be globals
inline fvec3 current_normals;
inline showcol_brush current_winding;
inline std::vector<showcol_brush> s_brushes;
inline cbrush_t* current_brush = 0;


inline void CM_ClearBrushes()
{

	s_brushes.clear();
	current_brush = 0;
	current_winding.windings.clear();
}