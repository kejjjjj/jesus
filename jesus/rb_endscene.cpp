#include "pch.hpp"

__declspec(naked) void RB_FixEndscene()
{
	static DWORD dst = 0x6496DD;
	__asm {
		jmp dst;
	}
}

void RB_DrawPolyInteriors(int n_points, std::vector<fvec3>& points, const BYTE* color, bool two_sided, bool depthTest, bool show)
{
	//partly copied from iw3xo :)
	if (n_points < 3)
		return;

	//static bool once = true;

	Material material = *rgp->whiteMaterial;

	//if (once) {
	//	memcpy(material.techniqueSet, rgp->whiteMaterial->techniqueSet, sizeof(MaterialTechniqueSet));
	//	memcpy(material.textureTable, rgp->whiteMaterial->textureTable, sizeof(MaterialTextureDef));
	//	memcpy(material.constantTable, rgp->whiteMaterial->constantTable, sizeof(MaterialConstantDef));
	//	memcpy(material.stateBitsTable, rgp->whiteMaterial->stateBitsTable, sizeof(GfxStateBits));
	//	once = false;
	//}
	static unsigned int loadBits[2] = { material.stateBitsTable->loadBits[0], material.stateBitsTable->loadBits[1] };
	static GfxStateBits bits = { .loadBits = { material.stateBitsTable->loadBits[0], material.stateBitsTable->loadBits[1] } };

	memcpy(material.stateBitsTable, rgp->whiteMaterial->stateBitsTable, sizeof(GfxStateBits));
	material.stateBitsTable = &bits;
	constexpr MaterialTechniqueType tech = MaterialTechniqueType::TECHNIQUE_UNLIT;
	static uint32_t ogBits = material.stateBitsTable->loadBits[1];

	if (gfxCmdBufState->origMaterial != &material || gfxCmdBufState->origTechType != tech) {
		if (tess->indexCount)
			RB_EndTessSurface();

		if (depthTest)
			material.stateBitsTable->loadBits[1] = 44;
		else
			material.stateBitsTable->loadBits[1] = ogBits;

		if (two_sided)
			material.stateBitsTable->loadBits[0] = 422072677;
		else
			material.stateBitsTable->loadBits[0] = 422089061;

		//material.stateBitsTable->loadBits[1] = 44;

		RB_BeginSurface(tech, &material);

	}
	if (n_points + tess->vertexCount > 5450 || tess->indexCount + 2 * (n_points - 2) > 1048576)// RB_CheckTessOverflow
	{
		RB_EndTessSurface();
		RB_BeginSurface(gfxCmdBufState->origTechType, gfxCmdBufState->origMaterial);
	}
	int idx = 0;

	for (; idx < n_points; ++idx) {
		//there should never be < 3 points
		//vec4_t plane;
		//PlaneFromPoints(plane, points[0], points[1], points[2]);

		vec3_t p = { points[idx].x, points[idx].y, points[idx].z };
		RB_SetPolyVertice(p, color, tess->vertexCount + idx, idx, 0);
	}

	for (idx = 2; idx < n_points; ++idx)
	{
		tess->indices[tess->indexCount + 0] = tess->vertexCount;
		tess->indices[tess->indexCount + 1] = (idx + tess->vertexCount);
		tess->indices[tess->indexCount + 2] = (idx + tess->vertexCount - 1);
		tess->indexCount += 3;
	}

	tess->vertexCount += n_points;

	RB_EndTessSurface();

}
int RB_AddDebugLine(GfxPointVertex* verts, char depthTest, const vec_t* start, vec_t* end, const BYTE* color, int vertCount)
{
	int _vc = vertCount;
	uint8_t _color[4]{ 0,0,0,0 };
	if (vertCount + 2 > 2725)
	{
		RB_DrawLines3D(vertCount / 2, 1, verts, depthTest);
		_vc = 0;
	}

	GfxPointVertex* vert = &verts[_vc];
	//if (color) {
	//	R_ConvertColorToBytes(color, vert->color);
	//}

	VectorCopy(color, vert->color);
	vert->color[3] = color[3];
	verts[_vc + 1].color[0] = vert->color[0];
	verts[_vc + 1].color[1] = vert->color[1];
	verts[_vc + 1].color[2] = vert->color[2];
	verts[_vc + 1].color[3] = vert->color[3];

	VectorCopy(start, vert->xyz);

	vert = &verts[_vc + 1];
	VectorCopy(end, vert->xyz);


	return _vc + 2;

	//return ((int(*)(GfxPointVertex *, char, const vec_t *, vec_t *, const vec_t *, int))0x658210)(verts, depthTest, start, end, color, vertCount);
}
void R_ConvertColorToBytes(const vec4_t in, uint8_t* out)
{
	//__asm
	//{
	//	lea edx, out;
	//	mov ecx, in;
	//	mov esi, 0x493530;
	//	call esi;
	//}
	((char(__fastcall*)(const float* in, uint8_t * out))0x493530)(in, out);

	return;
}
char RB_DrawLines3D(int count, int width, GfxPointVertex* verts, char depthTest)
{
	((char(__cdecl*)(int, int, GfxPointVertex*, char))0x613040)(count, width, verts, depthTest);
	return 1;

}
int RB_BeginSurface(MaterialTechniqueType tech, Material* material)
{
	int rval = 0;
	const static DWORD fnc = 0x61A220;
	__asm
	{
		mov edi, tech;
		mov esi, material;
		call fnc;
		mov rval, eax;
	}
	return rval;
}
void RB_EndTessSurface()
{
	((void(*)())0x61A2F0)();

}
void RB_SetPolyVertice(const vec3_t pos, const BYTE* col, const int vert, const int index, float* normal)
{
	VectorCopy(pos, tess->verts[vert].xyzw);
	//tess->verts[vert].color.packed = 0xFF00FFAA;
	tess->verts[vert].color.array[0] = col[0];
	tess->verts[vert].color.array[1] = col[1];
	tess->verts[vert].color.array[2] = col[2];
	tess->verts[vert].color.array[3] = col[3];

	//std::cout << "color: " << std::hex << tess->verts[vert].color.packed << '\n';

	switch (index)
	{
	case 0:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	case 1:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 2:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 1.0f;
		break;
	case 3:
		tess->verts[vert].texCoord[0] = 1.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;

	default:
		tess->verts[vert].texCoord[0] = 0.0f;
		tess->verts[vert].texCoord[1] = 0.0f;
		break;
	}

	tess->verts[vert].normal.packed = normal ? Vec3PackUnitVec(normal) : 1073643391;
}
char RB_DrawDebug(GfxViewParms* viewParms)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_RB_ENDSCENE);

	if(COD4X::getInstance().attempted_screenshot())
		return detour_func.cast_call<char(*)(GfxViewParms*)>(viewParms);


	decltype(auto) r = MovementRecorder::getInstance();

	r.RB_OnRenderPositions();

	//static std::vector<float> maxVerts(24);
	//static std::vector<Poly> polygons(1);

	//if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
	//	maxVerts.clear();
	//	polygons.clear();
	//	maxVerts.resize(24);
	//	polygons.resize(1);

	//	int idx = int(random(cm->numBrushes/2));
	//	cbrush_t* brush = &cm->brushes[idx];

	//	while (brush->numsides == 0) {
	//		idx = int(random(cm->numBrushes/2));
	//		brush = &cm->brushes[idx];
	//	}
	//	//Com_Printf("numsides: %i, index %i\n", brush->numsides, idx);
	//	//std::cout << idx << " -> bounds: " << (fvec3(brush->maxs) - fvec3(brush->mins)).abs() << '\n';
	//
	//	int verts = CM_BuildWindingsForBrush(&brush[idx], polygons, maxVerts);

	//	Poly* poly = &polygons.front();
	//	int i = 0;
	//	do
	//	{ 
	//		CM_DrawPoly(poly, vec4_t{1,0,0,1.f});
	//		++poly;
	//	} while (++i < brush->numsides + 6);

	//	VectorCopy(polygons.front().pts[0], ps_loc->origin);

	//}

	//if (polygons.size() > 1) {
	//	for (auto& i : polygons) {
	//		CM_DrawPoly(&i, vec4_t{ 0,1,1,0.7f });
	//	}
	//}

	RB_ShowCollision(viewParms);
		
	return detour_func.cast_call<char(*)(GfxViewParms*)>(viewParms);
}

void RB_DrawTriangleOutline(vec3_t points[3], vec4_t color, int width, bool depthTest)
{
	GfxPointVertex verts[6]{};

	BYTE c[4];

	R_ConvertColorToBytes(color, c);

	RB_AddDebugLine(verts, depthTest, points[0], points[1], c, 0);
	RB_AddDebugLine(verts, depthTest, points[0], points[2], c, 2);
	RB_AddDebugLine(verts, depthTest, points[1], points[2], c, 4);

	RB_DrawLines3D(3, width, verts, depthTest);

}
void R_AddDebugBox(const float* mins, const float* maxs, DebugGlobals* debugGlobalsEntry, float* color)
{
	__asm
	{
		push color;
		push debugGlobalsEntry;
		mov edx, maxs;
		mov eax, mins;
		mov esi, 0x60DC60;
		call esi;
		add esp, 0x8;
	}
}
void CM_DrawPoly(Poly* poly, float* color)
{
	__asm
	{
		push color;
		push poly;
		mov esi, 0x597200;
		call esi;
		add esp, 0x8;
	}
}
HRESULT R_DrawXModelSkinnedCached(GfxCmdBufSourceState* src, GfxCmdBufState* state, GfxModelSkinnedSurface* modelSurf)
{
	decltype(auto) detour_func = find_hook(hookEnums_e::HOOK_XMODEL_SKINNED);
	static LPDIRECT3DTEXTURE9 tex_Z;

	if (find_evar<bool>("Chams")->get() == false || COD4X::getInstance().attempted_screenshot())
		return detour_func.cast_call<HRESULT(*)(GfxCmdBufSourceState *, GfxCmdBufState*, GfxModelSkinnedSurface*)>(src, state, modelSurf);

	if (!tex_Z) {
		BYTE col[60]{};
		
		const auto R_GetD3D9TextureDataWithColor = [](BYTE r, BYTE g, BYTE b, BYTE* buffer) -> void {
			BYTE col[60] =
			{
				  0x42, 0x4D, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				  0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
				  0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
				  0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				  b, g, r, 0x00, 0x00, 0x00
			};

			memcpy_s(buffer, 60, &col, 60);
		};

		R_GetD3D9TextureDataWithColor(255, 0, 0, col);
		D3DXCreateTextureFromFileInMemory(cg::dx->device, (LPCVOID)&col, 60, &tex_Z);
	}

	cg::dx->device->SetRenderState(D3DRS_ZENABLE, false);
	cg::dx->device->SetTexture(0, tex_Z);
	detour_func.cast_call<HRESULT(*)(GfxCmdBufSourceState*, GfxCmdBufState*, GfxModelSkinnedSurface*)>(src, state, modelSurf);


	cg::dx->device->SetRenderState(D3DRS_ZENABLE, true);
	cg::dx->device->SetTexture(0, tex_Z);


	detour_func.cast_call<HRESULT(*)(GfxCmdBufSourceState*, GfxCmdBufState*, GfxModelSkinnedSurface*)>(src, state, modelSurf);

	cg::dx->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	return S_OK;
}