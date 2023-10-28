#pragma once
#include "pch.hpp"



void Cmd_ShowBrushes_f();

struct sc_winding_t
{
	std::vector<fvec3> points;
};

struct showcol_brush
{
	std::vector<sc_winding_t> windings;
	cbrush_t* brush;
};

inline std::list<showcol_brush> showcollision_brushes;

void CM_BuildAxialPlanes(float(*planes)[6][4], cbrush_t* brush);
void CM_GetWindingForBrushFace(unsigned int brushSide, cbrush_t* brush, Poly* polygon, const float(*planes)[4], signed int maxVerts);
int CM_BuildWindingsForBrush(cbrush_t* brush, std::vector<Poly>& outPolys, std::vector<float>& maxVerts);
void Phys_GetWindingForBrushFace2(unsigned int brushSideIndex, cbrush_t* brush, Poly* outPolys, signed int maxPolys, float(*axialPlanes)[6][4]);
int Phys_BuildWindingsForBrush(cbrush_t* brush, const float(*planes)[4], Poly* outPolys, float(*outVerts)[3]);

void RB_ShowCollision(GfxViewParms* viewParms);
void RB_RenderWinding(const showcol_brush& sb);
bool CM_BrushInView(const cbrush_t* brush, struct cplane_s* frustumPlanes, int numPlanes);
