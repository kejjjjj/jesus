#include "pch.hpp"

void CG_AdjustFrom640(float& x, float& y, float& w, float& h)
{
	const float scale = (float)cgs->refdef.width / 640.f;

	x *= scale;
	y *= scale;
	w *= scale;
	h *= scale;
}
Material* R_RegisterMaterial(const char* mtl)
{
	return ((Material * (*)(const char* mtl, int size))0x5F2A80)(mtl, sizeof(mtl));

}
Font_s* R_RegisterFont(const char* fontname)
{
	return ((Font_s * (*)(const char* fontname, int size))0x5F1EC0)(fontname, sizeof(fontname));
}
void R_DrawRect(const char* material, float x, float y, float w, float h, const float* color)
{
	CG_AdjustFrom640(x, y, w, h);
	Material* mat = R_RegisterMaterial(material);
	R_AddCmdDrawStretchPic(mat, (x), (y), (w), (h), 0, 0, 0, 0, color);


}
void R_AddCmdDrawStretchPic(Material* material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color)
{
	const static uint32_t R_AddCmdDrawStretchPic_func = 0x5F65F0;
	__asm
	{
		pushad;
		push	color;
		mov		eax, [material];
		sub		esp, 20h;
		fld		t1;
		fstp[esp + 1Ch];
		fld		s1;
		fstp[esp + 18h];
		fld		t0;
		fstp[esp + 14h];
		fld		s0;
		fstp[esp + 10h];
		fld		h;
		fstp[esp + 0Ch];
		fld		w;
		fstp[esp + 8h];
		fld		y;
		fstp[esp + 4h];
		fld		x;
		fstp[esp];
		call	R_AddCmdDrawStretchPic_func;
		add		esp, 24h;
		popad;
	}
}
char* __cdecl R_AddCmdDrawText(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{
	const static uint32_t R_AddCmdDrawText_func = 0x5F6B00;
	__asm
	{
		push	style;
		sub     esp, 14h;
		fld		rotation;
		fstp[esp + 10h];
		fld		yScale;
		fstp[esp + 0Ch];
		fld		xScale;
		fstp[esp + 8];
		fld		y;
		fstp[esp + 4];
		fld		x;
		fstp[esp];
		push	font;
		push	maxChars;
		push	text;
		mov		ecx, [color];
		call	R_AddCmdDrawText_func;
		add		esp, 24h;
	}
}
void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	const static uint32_t R_AddCmdDrawTextWithEffects_f = 0x5F6D30;

	return ((void(__cdecl*)(char* text, int maxChars, Font_s * font, float x, float y, float xScale, float yScale, float rotation,
		const float* color, int style, const float* glowColor, Material * fxMaterial, Material * fxMaterialGlow,
		int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration))0x5F6D30)(text, maxChars, font, x, y, xScale, yScale, rotation,
			color, style, glowColor, fxMaterial, fxMaterialGlow,
			fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);
}
void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration)
{
	Font_s* _font = R_RegisterFont(font);

	R_AddCmdDrawTextWithEffects(text, 0x7FFFFFFF, _font, x, y, xScale, yScale, rotation, color, style, glowColor, fxMaterial, fxMaterialGlow, fxBirthTime, fxLetterTime, fxDecayStartTime, fxDecayDuration);

}
ScreenPlacement* CG_GetScreenPlacement(int LocalClientNum)
{
	return (ScreenPlacement*)((68 * LocalClientNum + 14894040));
}
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, const char* material)
{
	Material* mat = R_RegisterMaterial(material);
	return CG_DrawRotatedPic(vertical, horizontal, scrPlace, x, y, w, h, angle, color, mat);
}
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, Material* material)
{
	ScrPlace_ApplyRect(&x, &w, &y, &h, scrPlace, horizontal, vertical);
	return CG_DrawRotatedPicPhysical(scrPlace, x, y, w, h, angle, color, material);
}
ScreenPlacement* ScrPlace_ApplyRect(float* x, float* w, float* y, float* h, ScreenPlacement* placement, int horizontal, int vertical)
{
	ScreenPlacement* result; // eax
	double _x; // st6
	double _w; // st6

	result = placement;
	switch (horizontal)
	{
	case 1:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewableMin[0];
		goto LABEL_9;
	case 2:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewportSize[0] * 0.5;
		goto LABEL_9;
	case 3:
		_x = placement->scaleVirtualToReal[0] * *x + placement->realViewableMax[0];
		goto LABEL_9;
	case 4:
		*x = placement->scaleVirtualToFull[0] * *x;
		_w = placement->scaleVirtualToFull[0];
		goto OOF;
	case 5:
		goto LABEL_11;
	case 6:
		*x = placement->scaleRealToVirtual[0] * *x;
		_w = placement->scaleRealToVirtual[0];
		goto OOF;
	case 7:
		_x = (placement->realViewableMax[0] + placement->realViewableMin[0]) * 0.5 + placement->scaleVirtualToReal[0] * *x;
		goto LABEL_9;
	default:
		_x = placement->scaleVirtualToReal[0] * *x + placement->subScreenLeft;// crash
	LABEL_9:
		*x = _x;
		_w = placement->scaleVirtualToReal[0];
	OOF:
		*w = _w * *w;
	LABEL_11:
		switch (vertical)
		{
		case 1:
			*y = *y * placement->scaleVirtualToReal[1] + placement->realViewableMin[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 2:
			*y = 0.5 * placement->realViewportSize[1] + *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 3:
			*y = *y * placement->scaleVirtualToReal[1] + placement->realViewableMax[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		case 4:
			*y = *y * placement->scaleVirtualToFull[1];
			*h = placement->scaleVirtualToFull[1] * *h;
			break;
		case 5:
			return result;
		case 6:
			*y = *y * placement->scaleRealToVirtual[1];
			*h = placement->scaleRealToVirtual[1] * *h;
			break;
		case 7:
			*y = 0.5 * (placement->realViewableMax[1] + placement->realViewableMin[1]) + *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		default:
			*y = *y * placement->scaleVirtualToReal[1];
			*h = placement->scaleVirtualToReal[1] * *h;
			break;
		}
		return result;
	}
}
void CG_DrawRotatedPicPhysical(ScreenPlacement* screenPlace, float x, float y, float w, float h, float angle, float* color, Material* material)
{

	double v8; // st7
	double v9; // st4
	double v10; // st3
	double v11; // st6
	double v12; // st2
	double v13; // st7
	double v14; // st5
	float v16; // [esp+0h] [ebp-44h] BYREF
	float v17; // [esp+Ch] [ebp-38h]
	float v18; // [esp+10h] [ebp-34h]
	float v19; // [esp+14h] [ebp-30h]
	float v20; // [esp+18h] [ebp-2Ch]
	float v21; // [esp+1Ch] [ebp-28h]
	float v22; // [esp+20h] [ebp-24h]
	float verts[8]; // [esp+24h] [ebp-20h] BYREF
	float wa; // [esp+50h] [ebp+Ch]
	float wb; // [esp+50h] [ebp+Ch]
	float wc; // [esp+50h] [ebp+Ch]
	float wd; // [esp+50h] [ebp+Ch]
	float we; // [esp+50h] [ebp+Ch]
	float wf; // [esp+50h] [ebp+Ch]
	float wg; // [esp+50h] [ebp+Ch]
	float wh; // [esp+50h] [ebp+Ch]
	float wi; // [esp+50h] [ebp+Ch]
	float wj; // [esp+50h] [ebp+Ch]
	float wk; // [esp+50h] [ebp+Ch]
	float wl; // [esp+50h] [ebp+Ch]
	float ha; // [esp+54h] [ebp+10h]

	v16 = angle;
	v17 = angle * 0.01745329238474369;
	angle = cos(v17);
	v16 = sin(v17);
	wa = w * 0.5;
	v17 = wa * screenPlace->scaleRealToVirtual[0];
	wb = 0.5 * h;
	ha = wb * screenPlace->scaleRealToVirtual[1];
	wc = x * screenPlace->scaleRealToVirtual[0];
	v8 = v17;
	v17 = wc + v17;
	wd = screenPlace->scaleRealToVirtual[1] * y;
	v18 = wd + ha;
	v19 = v8 * angle;
	v20 = v8 * v16;
	v21 = v16 * ha;
	v22 = angle * ha;
	v9 = v17 - v19 * 1.0;
	v10 = v21 * -1.0;
	we = v9 - v10;
	verts[0] = we * screenPlace->scaleVirtualToReal[0];
	v11 = v18 - 1.0 * v20;
	v12 = -1.0 * v22;
	wf = v12 + v11;
	verts[1] = wf * screenPlace->scaleVirtualToReal[1];
	v13 = v17 + v19;
	wg = v13 - v10;
	verts[2] = wg * screenPlace->scaleVirtualToReal[0];
	v14 = v20 + v18;
	wh = v12 + v14;
	verts[3] = wh * screenPlace->scaleVirtualToReal[1];
	wi = v13 - v21;
	verts[4] = wi * screenPlace->scaleVirtualToReal[0];
	wj = v14 + v22;
	verts[5] = wj * screenPlace->scaleVirtualToReal[1];
	wk = v9 - v21;
	verts[6] = wk * screenPlace->scaleVirtualToReal[0];
	wl = v22 + v11;
	verts[7] = wl * screenPlace->scaleVirtualToReal[1];

	__asm
	{
		mov edx, material;
		push material;
		lea edx, verts;
		mov ecx, color;
		mov esi, 0x5F7230;
		call esi;
		add esp, 0x4;
	}

}
void R_DrawText(const char* text, const char* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{

	Font_s* _font = R_RegisterFont(font);

	R_AddCmdDrawText(text, 0x7FFFFFFF, _font, (x), (y), xScale, yScale, rotation, color, style);

}
void R_DrawText(const char* text, float x, float y, float xScale, float yScale, float rotation, float* color, int style)
{
	R_DrawText(text, "fonts/smalldevfont", x, y, xScale, yScale, rotation, color, style);
}

std::optional<ivec2> WorldToScreen(const fvec3& location)
{
	const refdef_s* refdef = &cgs->refdef;

	const int centerX = refdef->width / 2;
	const int centerY = refdef->height / 2;

	const fvec3 vright = refdef->viewaxis[1];
	const fvec3 vup = refdef->viewaxis[2];
	const fvec3 vfwd = refdef->viewaxis[0];

	const fvec3 vLocal = location - refdef->vieworg;
	fvec3 vTransform;

	vTransform.x = vLocal.dot(vright);
	vTransform.y = vLocal.dot(vup);
	vTransform.z = vLocal.dot(vfwd);

	if (vTransform.z < 0.01) {
		return std::nullopt;
	}
	ivec2 out;

	out.x = centerX * (1 - (vTransform.x / refdef->tanHalfFovX / vTransform.z));
	out.y = centerY * (1 - (vTransform.y / refdef->tanHalfFovY / vTransform.z));


	if (vTransform.z > 0)
		return out;

	return std::nullopt;
}

void R_DrawTriangle(vec3_t pointA, vec3_t pointB, vec3_t pointC, vec4_t col)
{
	std::optional<ivec2> a, b, c;
	
	a = WorldToScreen(pointA);
	b = WorldToScreen(pointB);
	c = WorldToScreen(pointC);


	if ((a && b && c) == false) {
		return;
	}

	ImGui::GetBackgroundDrawList()->AddTriangleFilled(a.value(), b.value(), c.value(), IM_COL32(col[0], col[1], col[2], col[3]));

}
void R_DrawLine(vec3_t pointA, vec3_t pointB, vec4_t col)
{
	std::optional<ivec2> a, b;

	a = WorldToScreen(pointA);
	b = WorldToScreen(pointB);


	if ((a && b) == false) {
		return;
	}

	ImGui::GetBackgroundDrawList()->AddLine(a.value(), b.value(), IM_COL32(col[0], col[1], col[2], col[3]), 3);

}

box_s::box_s(const fvec3& origin, const fvec3& mins, const fvec3& maxs)
{
	//box_s box{};

	fvec3 lowA, lowB, lowC, lowD;
	fvec3 highA, highB, highC, highD;

	lowA.x = origin.x - mins.x;
	lowA.y = origin.y - mins.y;
	lowA.z = origin.z - mins.z;

	lowB.x = origin.x + mins.x;
	lowB.y = origin.y + mins.y;
	lowB.z = origin.z - mins.z;

	lowC.x = origin.x - mins.x;
	lowC.y = origin.y + mins.y;
	lowC.z = origin.z - mins.z;

	lowD.x = origin.x + mins.x;
	lowD.y = origin.y - mins.y;
	lowD.z = origin.z - mins.z;

	highA.x = lowA.x;
	highA.y = lowA.y;
	highA.z = origin.z + maxs.z;

	highB.x = lowB.x;
	highB.y = lowB.y;
	highB.z = origin.z + maxs.z;

	highC.x = lowC.x;
	highC.y = lowC.y;
	highC.z = origin.z + maxs.z;

	highD.x = lowD.x;
	highD.y = lowD.y;
	highD.z = origin.z + maxs.z;

	this->lowA = WorldToScreen(lowA);
	this->lowB = WorldToScreen(lowB);
	this->lowC = WorldToScreen(lowC);
	this->lowD = WorldToScreen(lowD);

	this->highA= WorldToScreen(highA);
	this->highB = WorldToScreen(highB);
	this->highC = WorldToScreen(highC);
	this->highD = WorldToScreen(highD);

	//return box;
}
void box_s::R_DrawConstructedBoxEdges(vec4_t col) const
{
	if (!ImGui::GetCurrentContext())
		return;

	auto& low_a = lowA.has_value() ? lowA.value() : 0;
	auto& low_b = lowB.has_value() ? lowB.value() : 0;
	auto& low_c = lowC.has_value() ? lowC.value() : 0;
	auto& low_d = lowD.has_value() ? lowD.value() : 0;

	auto& high_a = highA.has_value() ? highA.value() : 0;
	auto& high_b = highB.has_value() ? highB.value() : 0;
	auto& high_c = highC.has_value() ? highC.value() : 0;
	auto& high_d = highD.has_value() ? highD.value() : 0;


	if (lowA && lowC)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_a.x, low_a.y), ImVec2(low_c.x, low_c.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowB && lowD)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_b.x, low_b.y), ImVec2(low_d.x, low_d.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowC && lowB)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_c.x, low_c.y), ImVec2(low_b.x, low_b.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowD && lowA)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_d.x, low_d.y), ImVec2(low_a.x, low_a.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (highA && highC)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(high_a.x, high_a.y), ImVec2(high_c.x, high_c.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (highB && highD)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(high_b.x, high_b.y), ImVec2(high_d.x, high_d.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (highC && highB)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(high_c.x, high_c.y), ImVec2(high_b.x, high_b.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (highD && highA)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(high_d.x, high_d.y), ImVec2(high_a.x, high_a.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowA && highA)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_a.x, low_a.y), ImVec2(high_a.x, high_a.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowB && highB)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_b.x, low_b.y), ImVec2(high_b.x, high_b.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowC && highC)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_c.x, low_c.y), ImVec2(high_c.x, high_c.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);

	if (lowD && highD)
		ImGui::GetBackgroundDrawList()->AddLine(ImVec2(low_d.x, low_d.y), ImVec2(high_d.x, high_d.y), IM_COL32(col[0], col[1], col[2], col[3]), 1.f);
}
void box_s::R_DrawConstructedBox(vec4_t col) const
{
	if (!ImGui::GetCurrentContext())
		return;

	auto& low_a = lowA.has_value() ? lowA.value() : 0;
	auto& low_b = lowB.has_value() ? lowB.value() : 0;
	auto& low_c = lowC.has_value() ? lowC.value() : 0;
	auto& low_d = lowD.has_value() ? lowD.value() : 0;

	auto& high_a = highA.has_value() ? highA.value() : 0;
	auto& high_b = highB.has_value() ? highB.value() : 0;
	auto& high_c = highC.has_value() ? highC.value() : 0;
	auto& high_d = highD.has_value() ? highD.value() : 0;

	if (lowA && highC && lowC)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_a.x, low_a.y), ImVec2(high_c.x, high_c.y), ImVec2(low_c.x, low_c.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (lowA && highA && highC)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_a.x, high_a.y), ImVec2(low_a.x, low_a.y), ImVec2(high_c.x, high_c.y), IM_COL32(col[0], col[1], col[2], col[3]));

	if (lowB && highD && lowD)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_b.x, low_b.y), ImVec2(high_d.x, high_d.y), ImVec2(low_d.x, low_d.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (lowB && highB && highD)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_b.x, high_b.y), ImVec2(low_b.x, low_b.y), ImVec2(high_d.x, high_d.y), IM_COL32(col[0], col[1], col[2], col[3]));

	if (lowC && highB && lowB)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_c.x, low_c.y), ImVec2(high_b.x, high_b.y), ImVec2(low_b.x, low_b.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (lowC && highC && highB)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_c.x, high_c.y), ImVec2(low_c.x, low_c.y), ImVec2(high_b.x, high_b.y), IM_COL32(col[0], col[1], col[2], col[3]));

	if (lowD && highA && lowA)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_d.x, low_d.y), ImVec2(high_a.x, high_a.y), ImVec2(low_a.x, low_a.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (lowD && highD && highA)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_d.x, high_d.y), ImVec2(low_d.x, low_d.y), ImVec2(high_a.x, high_a.y), IM_COL32(col[0], col[1], col[2], col[3]));

	if (highA && highB && highC)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_a.x, high_a.y), ImVec2(high_b.x, high_b.y), ImVec2(high_c.x, high_c.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (highA && highB && highD)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(high_a.x, high_a.y), ImVec2(high_b.x, high_b.y), ImVec2(high_d.x, high_d.y), IM_COL32(col[0], col[1], col[2], col[3]));

	if (lowA && lowB && lowC)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_a.x, low_a.y), ImVec2(low_b.x, low_b.y), ImVec2(low_c.x, low_c.y), IM_COL32(col[0], col[1], col[2], col[3]));
	if (lowA && lowB && lowD)
		ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(low_a.x, low_a.y), ImVec2(low_b.x, low_b.y), ImVec2(low_d.x, low_d.y), IM_COL32(col[0], col[1], col[2], col[3]));
}