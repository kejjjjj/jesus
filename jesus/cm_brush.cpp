#include "pch.hpp"
void Cmd_ShowBrushes_f()
{
	if (cmd_args->argc[cmd_args->nesting] == 0 || cmd_args->argc[cmd_args->nesting] > 2) {
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "usage: showbrush <material> or showbrush (will clear everything)");
		return;
	}

	if (cmd_args->argc[cmd_args->nesting] == 1) {
		showcollision_brushes.clear();
		Com_Printf(CON_CHANNEL_CONSOLEONLY, "^2cleared!\n");
		return;
	}

	auto arg = *(cmd_args->argv[cmd_args->nesting] + 1);


	for (int bidx = 0; bidx < cm->numBrushes; bidx++) {

		cbrush_t* brush = &cm->brushes[bidx];

		if (!brush || std::string(cm->materials[brush->axialMaterialNum[0][0]].material).find(arg) == std::string::npos)
			continue;

		std::vector<float> maxVerts(24);
		std::vector<Poly> polygons(1);
		int verts = CM_BuildWindingsForBrush(brush, polygons, maxVerts);

		std::cout << bidx << '\n';

		Poly* poly = &polygons.front();


		showcol_brush b;

		int i = 0;
		do
		{
			sc_winding_t w;

			for (int p = 0; p < poly->ptCount; p++) {
				
				w.points.push_back(fvec3{ poly->pts[p][0], poly->pts[p][1], poly->pts[p][2] });
			}


			b.brush = brush;
			if(poly->ptCount)
				b.windings.push_back((w));
			++poly;
		} while (++i < brush->numsides + 6);

		showcollision_brushes.push_back(b);
	}

	std::cout << "done!\n";

}
void CM_BuildAxialPlanes(float(*planes)[6][4], cbrush_t* brush)
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
	
	maxVerts.resize(200);

	//std::cout <<  "bounds: " << (fvec3(brush->maxs) - fvec3(brush->mins)).abs() << '\n';

	do {
		outPolys[sides].pts = (float(*)[3])&maxVerts.data()[3*verts];



		Phys_GetWindingForBrushFace2(sides, brush, &outPolys[sides], 1024 - verts, (float(*)[6][4])planes);

		//std::cout << "iteration " << sides << '\n';

		verts += outPolys[sides].ptCount;
		++sides;

		//maxVerts.insert(maxVerts.end(), outPolys[sides-1].ptCount, 0.f);
		//maxVerts.resize(verts * 3 + 1);
		//std::cout << "new size is " << maxVerts.size() << " and it should be " << verts * 3 + 1<< '\n';
		outPolys.resize(sides+1);


	} while (sides < brush->numsides + 6);

	outPolys.pop_back();

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
	if (showcollision_brushes.empty())
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

	for(auto& i : showcollision_brushes)
		if (CM_BrushInView(i.brush, frustum_planes, 5)) {
			RB_RenderWinding(i);
		}

}
void RB_RenderWinding(const showcol_brush& sb)
{
	//if (sb.brush->get_origin().dist(clients->cgameOrigin) > 2000)
	//	return;

	for (auto& i : sb.windings)
		RB_DrawCollisionPoly(i.points.size(), (float(*)[3])i.points.data(), vec4_t{ 0,1,1,0.3f });

}

void RB_DrawCollisionPoly(int numPoints, float(*points)[3], const float* colorFloat)
{
	uint8_t c[4];
	std::vector<fvec3> pts;
	const auto depthtest = true;

	R_ConvertColorToBytes(colorFloat, c);

	for (int i = 0; i < numPoints; i++)
		pts.push_back(points[i]);


	//GfxPointVertex verts[4]{};

	//for (int i = 0; i < numPoints -1; i++) {


	//	RB_AddDebugLine(verts, depthtest->get(), points[i], (float*)points[i + 1], c, 0);
	//	RB_DrawLines3D(1, 3, verts, depthtest->get());

	//}

	RB_DrawPolyInteriors(numPoints, pts, c, true, depthtest);

}