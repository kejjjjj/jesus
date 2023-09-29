#pragma once

#include "pch.hpp"

using namespace cg;
void CG_AdjustFrom640(float& x, float& y, float& w, float& h);
Material* R_RegisterMaterial(const char* fontname);
Font_s* R_RegisterFont(const char* fontname);

char* __cdecl R_AddCmdDrawText(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style);

void R_AddCmdDrawStretchPic(Material* material, float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color);
void R_DrawRect(const char* material, float x, float y, float w, float h, const float* color);
void R_DrawText(const char* text, const char* font, float x, float y, float xScale, float yScale, float rotation, float* color, int style);
void R_DrawText(const char* text, float x, float y, float xScale, float yScale, float rotation, float* color, int style);

void R_AddCmdDrawTextWithEffects(char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void R_AddCmdDrawTextWithEffects(char* text, const char* font, float x, float y, float xScale, float yScale, float rotation,
	const float* color, int style, const float* glowColor, Material* fxMaterial, Material* fxMaterialGlow,
	int fxBirthTime, int fxLetterTime, int fxDecayStartTime, int fxDecayDuration);

void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, Material* material);
void CG_DrawRotatedPic(int vertical, int horizontal, ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, const char* material);

ScreenPlacement* ScrPlace_ApplyRect(float* x, float* w, float* y, float* h, ScreenPlacement* placement, int horizontal, int vertical);
void CG_DrawRotatedPicPhysical(ScreenPlacement* scrPlace, float x, float y, float w, float h, float angle, float* color, Material* material);

ScreenPlacement* CG_GetScreenPlacement(int LocalClientNum);

std::optional<ivec2> WorldToScreen(const fvec3& location);

void R_DrawTriangle(vec3_t pointA, vec3_t pointB, vec3_t pointC, vec4_t col);
void R_DrawLine(vec3_t pointA, vec3_t pointB, vec4_t col);

struct box_s
{
	box_s(const fvec3& origin, const fvec3& mins, const fvec3& maxs);

	void R_DrawConstructedBox(vec4_t col) const;
	void R_DrawConstructedBoxEdges(vec4_t col) const;

private:

	std::optional<ivec2> lowA;
	std::optional<ivec2> lowB;
	std::optional<ivec2> lowC;
	std::optional<ivec2> lowD;

	std::optional<ivec2> highA;
	std::optional<ivec2> highB;
	std::optional<ivec2> highC;
	std::optional<ivec2> highD;

	bool lowA_valid;
	bool lowB_valid;
	bool lowC_valid;
	bool lowD_valid;

	bool highA_valid;
	bool highB_valid;
	bool highC_valid;
	bool highD_valid;

};