#pragma once

#include "pch.hpp"

int VectorCompare(const vec3_t v1, const vec3_t v2);
vec_t VectorLength(const vec3_t v);
vec_t VectorLengthSquared(const vec3_t v);
vec_t Distance(const vec3_t p1, const vec3_t p2);
vec_t DistanceSquared(const vec3_t p1, const vec3_t p2);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
vec_t VectorNormalize(vec3_t v);       // returns vector length
void VectorNormalizeFast(vec3_t v);     // does NOT return vector length, uses rsqrt approximation
vec_t VectorNormalize2(const vec3_t v, vec3_t out);
void VectorInverse(vec3_t v);

void MatrixTransformVector43(const float* in1, const float(*in2)[3], float* out);
int BoxOnPlaneSide(const vec3_t emins, const vec3_t emaxs, struct cplane_s* p);

void BuildFrustumPlanes(const struct GfxViewParms* viewParms, cplane_s* frustumPlanes);
float random(const float range); //0 -> HI
float random(const float min, const float range); //LO -> HI

void SetPlaneSignbits(cplane_s* out);


inline int Vec3PackUnitVec(float* v) {
	int r = 0;
	__asm
	{
		mov eax, v;
		mov esi, 0x5645A0;
		call esi;
		mov r, eax;
	}
	return r;
}