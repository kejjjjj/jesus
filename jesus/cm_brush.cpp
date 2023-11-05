#include "pch.hpp"

SimplePlaneIntersection pts[1024];


void GetBrushPolys(cbrush_t* brush, float(*outPlanes)[4])
{
	int planeCount = BrushToPlanes(brush, outPlanes);
	int intersections = GetPlaneIntersections((const float**)outPlanes, planeCount, pts);
	adjacencyWinding_t windings[40]{};

	int verts = 0;
	int intersection = 0;

	current_brush = brush;
	do {
		current_normals = outPlanes[intersection];
		if (auto r = BuildBrushdAdjacencyWindingForSide(intersections, pts, outPlanes[intersection], intersection, &windings[intersection])) {
			verts += r->numsides;
		}

		++intersection;

	} while (intersection < planeCount);

	s_brushes.push_back(current_winding);
	current_winding.windings.clear();

}
void Cmd_ShowBrushes_f(char* arg)
{
	float planes[30][4]{};

	int found = 0;
	s_brushes.clear();
	for (int bidx = 0; bidx < cm->numBrushes; bidx++) {

		cbrush_t* brush = &cm->brushes[bidx];

		if (!brush || std::string(cm->materials[brush->axialMaterialNum[0][0]].material).find(arg) == std::string::npos)
			continue;

		++found;
		int planeCount = BrushToPlanes(brush, planes);
		int intersections = GetPlaneIntersections((const float**)planes, planeCount, pts);
		adjacencyWinding_t windings[32]{};

		int verts = 0;
		int intersection = 0;

		current_brush = brush;
		do {
			current_normals = planes[intersection];
			if (auto r = BuildBrushdAdjacencyWindingForSide(intersections, pts, planes[intersection], intersection, &windings[intersection])) {
				verts += r->numsides;
			}

			++intersection;

		} while (intersection < planeCount);
		s_brushes.push_back(current_winding);
		current_winding.windings.clear();

	}
	//std::cout << "loaded " << s_brushes.size() << " brushes while we found a total of " << found << " brushes!\n";

}
void CM_BuildAxialPlanes(float(*planes)[6][4], const cbrush_t* brush)
{

	(*planes)[0][0] = -1.0;
	(*planes)[0][1] = 0.0;
	(*planes)[0][2] = 0.0;
	(*planes)[0][3] = -brush->mins[0];
	(*planes)[1][0] = 1.0;
	(*planes)[1][1] = 0.0;
	(*planes)[1][2] = 0.0;
	(*planes)[1][3] = brush->maxs[0];
	(*planes)[2][0] = 0.0;
	(*planes)[2][2] = 0.0;
	(*planes)[2][1] = -1.0;
	(*planes)[2][3] = -brush->mins[1];
	(*planes)[3][0] = 0.0;
	(*planes)[3][2] = 0.0;
	(*planes)[3][1] = 1.0;
	(*planes)[3][3] = brush->maxs[1];
	(*planes)[4][0] = 0.0;
	(*planes)[4][1] = 0.0;
	(*planes)[4][2] = -1.0;
	(*planes)[4][3] = -brush->mins[2];
	(*planes)[5][0] = 0.0;
	(*planes)[5][1] = 0.0;
	(*planes)[5][2] = 1.0;
	(*planes)[5][3] = brush->maxs[2];
}
void CM_GetWindingForBrushFace(unsigned int brushSide, cbrush_t* brush, Poly* polygon, const float(*planes)[4], signed int maxVerts)
{
	static const DWORD f = 0x599560;
	__asm
	{
		mov eax, brushSide;
		mov edx, brush;
		mov esi, polygon;
		push maxVerts;
		push planes;
		call f;
		add esp, 0x8;
	}

}
int CM_BuildWindingsForBrush(cbrush_t* brush, std::vector<Poly>& outPolys, std::vector<float>& maxVerts)
{
	float planes[6][4]{};
	CM_BuildAxialPlanes((float(*)[6][4])planes, brush);
	int verts = 0;
	int sides = 0;
	int numPoints = 0;
	do {
		outPolys[sides].pts = (float(*)[3])&maxVerts.data()[3*verts];
		Phys_GetWindingForBrushFace2(sides, brush, &outPolys[sides], 1024 - verts, (float(*)[6][4])planes);
		verts += outPolys[sides].ptCount;
		
		if (outPolys[sides].ptCount)
			numPoints++;

		if (sides == NULL) {
			std::cout << std::format("{} edgeCount[0][{}] = {} with material {}\n", 
				verts == NULL ? "FAIL: " : "SUCCESS: ",
				0, 
				(int)brush->edgeCount[0][0], 
				cm->materials[brush->axialMaterialNum[0][0]].material);
		}
		++sides;
		outPolys.resize(sides+1);

	} while (sides < brush->numsides + 6);
	outPolys.pop_back();

	if (numPoints)
		return verts;

	verts = 0;
	sides = 0;

	do {
		outPolys[sides].pts = (float(*)[3]) &maxVerts.data()[3 * verts];
		CM_GetWindingForBrushFace(sides, brush, &outPolys[sides], planes, 256 - verts);
		verts += outPolys[sides].ptCount;

		if (outPolys[sides].ptCount)
			numPoints++;

		++sides;
		outPolys.resize(sides + 1);

	} while (sides < brush->numsides + 6);

	return verts;

}

void Phys_GetWindingForBrushFace2(unsigned int brushSideIndex, cbrush_t* brush, Poly* outPolys, signed int maxPolys, float(*axialPlanes)[6][4])
{
	__asm
	{
		mov eax, brushSideIndex;
		push axialPlanes;
		push maxPolys;
		push outPolys;
		push brush;
		mov esi, 0x5998E0;
		call esi;
		add esp, 16;
	}
}

bool CM_BrushInView(const cbrush_t* brush, struct cplane_s* frustumPlanes, int numPlanes)
{
	if (numPlanes <= 0)
		return 1;

	cplane_s* plane = frustumPlanes;
	int idx = 0;
	while ((BoxOnPlaneSide(brush->mins, brush->maxs, plane) & 1) != 0) {
		++plane;
		++idx;

		if (idx >= numPlanes)
			return 1;
	}

	return 0;
}

void RB_ShowCollision(GfxViewParms* viewParms)
{
	if (s_brushes.empty() && cm_terrainpoints.empty())
		return;

	cplane_s frustum_planes[6];

	BuildFrustumPlanes(viewParms, frustum_planes);

	frustum_planes[5].normal[0] = -frustum_planes[4].normal[0];
	frustum_planes[5].normal[1] = -frustum_planes[4].normal[1];
	frustum_planes[5].normal[2] = -frustum_planes[4].normal[2];

	frustum_planes[5].dist = -frustum_planes[4].dist - 2000;
	auto plane = &frustum_planes[5];

	char signbit = 0;

	if (plane->normal[0] != 1.f) {
		if (plane->normal[1] == 1.f)
			signbit = 1;
		else {
			signbit = plane->normal[2] == 1.f ? 2 : 3;
		}
	}

	plane->type = signbit;

	SetPlaneSignbits(plane);

	for (auto& i : s_brushes)
		if (CM_BrushInView(i.brush, frustum_planes, 5)) {
			RB_RenderWinding(i);
		}

	for (auto& i : cm_terrainpoints) {
		CM_ShowTerrain(&i, frustum_planes);
	}

}
void RB_RenderWinding(const showcol_brush& sb)
{
	if (sb.brush->get_origin().dist(clients->cgameOrigin) > find_evar<float>("Draw Distance")->get())
		return;

	bool only_bounces = find_evar<bool>("Only Bounces")->get();

	for (auto& i : sb.windings) {

		if (only_bounces && i.is_bounce == false)
			continue;

		RB_DrawCollisionPoly(i.points.size(), (float(*)[3])i.points.data(), vec4_t{ 0,1,1,0.3f });
	}


}

void RB_DrawCollisionPoly(int numPoints, float(*points)[3], const float* colorFloat)
{
	uint8_t c[4];
	std::vector<fvec3> pts;
	const auto depthtest = find_evar<bool>("Depth Test")->get();

	R_ConvertColorToBytes(colorFloat, c);

	for (int i = 0; i < numPoints; i++)
		pts.push_back(points[i]);


	GfxPointVertex verts[4]{};

	//for (int i = 0; i < numPoints -1; i++) {


	//	RB_AddDebugLine(verts, depthtest, points[i], (float*)points[i + 1], c, 0);
	//	RB_DrawLines3D(1, 3, verts, depthtest);

	//}

	RB_DrawPolyInteriors(numPoints, pts, c, true, depthtest);

}
void CM_GetPlaneVec4Form(const cbrushside_t* sides, const float(*axialPlanes)[4], int index, float* expandedPlane)
{
	if (index >= 6) {
		cplane_s* plane = sides[index - 6].plane;

		expandedPlane[0] = plane->normal[0];
		expandedPlane[1] = plane->normal[1];
		expandedPlane[2] = plane->normal[2];
		expandedPlane[3] = plane->dist;
		return;
	}

	const float* plane = axialPlanes[index];

	*expandedPlane = plane[0];
	expandedPlane[1] = plane[1];
	expandedPlane[2] = plane[2];
	expandedPlane[3] = plane[3];

}
void CM_ReverseWinding(winding_t* w)
{

	for (int i = 0; i < w->numpoints; i++)
	{

		const float temp[3] =
		{
			w->p[i][0], w->p[i][1], w->p[i][2]
		};

		VectorCopy(w->p[w->numpoints - 1 - i], w->p[i]);
		VectorCopy(temp, w->p[w->numpoints - 1 - i]);


	}
}
bool PlaneFromPoints(vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c)
{
	vec3_t d1, d2;

	VectorSubtract(b, a, d1);
	VectorSubtract(c, a, d2);


	CrossProduct(d2, d1, plane);

	float len = VectorLength(plane);

	if (!len)
		return false;

	if (len < 2.f) {
		if (VectorLength(d1) * VectorLength(d2) * 0.0000010000001f >= len) {
			VectorSubtract(c, b, d1);
			VectorSubtract(a, b, d2);

			CrossProduct(d2, d1, plane);

			if (VectorLength(d1) * VectorLength(d2) * 0.0000010000001f >= len) {
				return false;
			}

		}
	}

	if (VectorNormalize(plane) == 0) {
		return 0;
	}

	plane[3] = DotProduct(a, plane);
	return 1;
}
int GetPlaneIntersections(const float** planes, int planeCount, SimplePlaneIntersection* OutPts)
{
	int r = 0;
	__asm
	{
		push OutPts;
		push planeCount;
		push planes;
		mov esi, 0x58FB00;
		call esi;
		add esp, 12;
		mov r, eax;
	}

	return r;
}
int BrushToPlanes(const cbrush_t* brush, float(*outPlanes)[4])
{
	float planes[6][4];
	CM_BuildAxialPlanes((float(*)[6][4])planes, brush);
	int i = 0;
	do {
		CM_GetPlaneVec4Form(brush->sides, planes, i, outPlanes[i]);

	} while (++i < brush->numsides + 6);

	return i;
}
vec_t   WindingArea(winding_t* w)
{
	int i;
	vec3_t d1, d2, cross;
	vec_t total;

	total = 0;
	for (i = 2; i < w->numpoints; i++)
	{
		VectorSubtract(w->p[i - 1], w->p[0], d1);
		VectorSubtract(w->p[i], w->p[0], d2);
		CrossProduct(d1, d2, cross);
		total += 0.5 * VectorLength(cross);
	}
	return total;
}
void WindingPlane(winding_t* w, vec3_t normal, vec_t* dist) {
	vec3_t v1, v2;

	VectorSubtract(w->p[1], w->p[0], v1);
	VectorSubtract(w->p[2], w->p[0], v2);
	CrossProduct(v2, v1, normal);
	VectorNormalize2(normal, normal);
	*dist = DotProduct(w->p[0], normal);

}
void CheckWinding(winding_t* w) {
	int i, j;
	vec_t* p1, * p2;
	vec_t d, edgedist;
	vec3_t dir, edgenormal, facenormal;
	vec_t area;
	vec_t facedist;

	if (w->numpoints < 3) {
		return Com_Printf("CheckWinding: %i points\n", w->numpoints);
	}

	area = WindingArea(w);
	if (area < 1) {
		return Com_Printf("CheckWinding: %f area\n", area);
	}

	WindingPlane(w, facenormal, &facedist);

	for (i = 0; i < w->numpoints; i++)
	{
		p1 = w->p[i];

		//for (j = 0; j < 3; j++)
		//	if (p1[j] > MAX_MAP_BOUNDS || p1[j] < -MAX_MAP_BOUNDS) {
		//		Com_Error(ERR_DROP, "CheckFace: MAX_MAP_BOUNDS: %f", p1[j]);
		//	}

		j = i + 1 == w->numpoints ? 0 : i + 1;

		// check the point is on the face plane
		d = DotProduct(p1, facenormal) - facedist;
		if (d < -ON_EPSILON || d > ON_EPSILON) {
			return Com_Printf("CheckWinding: point off plane\n");
		}

		// check the edge isnt degenerate
		p2 = w->p[j];
		VectorSubtract(p2, p1, dir);

		if (VectorLength(dir) < ON_EPSILON) {
			return Com_Printf("CheckWinding: degenerate edge\n");
		}

		CrossProduct(facenormal, dir, edgenormal);
		VectorNormalize2(edgenormal, edgenormal);
		edgedist = DotProduct(p1, edgenormal);
		edgedist += ON_EPSILON;

		// all other points must be on front side
		for (j = 0; j < w->numpoints; j++)
		{
			if (j == i) {
				continue;
			}
			d = DotProduct(w->p[j], edgenormal);
			if (d > edgedist) {
				return Com_Printf("CheckWinding: non-convex\n");
			}
		}
	}
}
adjacencyWinding_t* BuildBrushdAdjacencyWindingForSide(int ptCount, SimplePlaneIntersection* pts, float* sideNormal, int planeIndex, adjacencyWinding_t* optionalOutWinding)
{
	adjacencyWinding_t* r = 0;

	__asm
	{
		mov edx, ptCount;
		mov ecx, pts;
		push optionalOutWinding;
		push planeIndex;
		push sideNormal;
		mov esi, 0x57D500;
		call esi;
		add esp, 12;
		mov r, eax;
	}

	return r;
}
void __cdecl wtf(adjacencyWinding_t* w, float* a, float* b, float* c, float* points)
{
	float plane[4];
	PlaneFromPoints(plane, a, b, c);
	


	if ((DotProduct(plane, current_normals) < 0.f)) {
		int* sides = w->sides;
		int* end = &w->sides[w->numsides];

		if (w->sides < end)
		{
			do
			{
				int temp = *sides;
				*sides = *end;
				*end-- = temp;
				++sides;
			} while (sides < end);
		}
	}


	std::vector<fvec3> winding_points;

	for (int winding = 0; winding < w->numsides; winding++) {
		winding_points.push_back({ &points[winding*3]});
	}

	current_winding.windings.push_back({ sc_winding_t{ winding_points } });
	current_winding.brush = current_brush;

	//PlaneFromPoints(plane, winding_points[0], winding_points[1], winding_points[2]);

	current_winding.windings.back().is_bounce = current_normals[2] >= 0.3f && current_normals[2] <= 0.7f;

}

__declspec(naked) void stealerino_test()
{
	static const DWORD lbl = 0x57D87A;
	__asm
	{
		mov eax, [esp + 1Ch];
		lea edx, [eax + eax * 2];
		mov eax, [esp + 20h];
		lea ecx, [eax + eax * 2];
		mov eax, [esp + 14h];
		lea esi, [eax + eax * 2];

		lea edx, [esp + edx * 4 + 00003038h]; //a
		lea esi, [esp + esi * 4 + 00003038h]; //b
		lea ecx, [esp + ecx * 4 + 00003038h]; //c
		
		lea eax, [esp + 00003048h - 16];

		push eax;
		push ecx;
		push esi;
		push edx;
		push ebx;

		call wtf;
		add esp, 20;

		xor eax, eax;
		pop edi;
		pop esi;
		pop ebp;
		pop ebx;
		add esp, 6028h;
		retn;

		//fld dword ptr[ebp + 04h];
		//fmul dword ptr[esp + 2Ch];
		//fld dword ptr[ebp + 00h];
		//fmul dword ptr[esp + 28h];
		//jmp lbl;

	}
}