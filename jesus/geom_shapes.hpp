#pragma once

#include "pch.hpp"

std::list<fvec3> Geom_CreateBall(const fvec3& ref_org, const float radius, const int32_t latitudeSegments, const int32_t longitudeSegments);
//std::vector<fvec3> Geom_CreatePyramid(const fvec3& ref_org, const fvec3& size, float rotation);
std::vector<fvec3> Geom_CreatePyramid(const fvec3& ref_org, const fvec3& size, float rotation);