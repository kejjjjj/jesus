#include "pch.hpp"

std::list<fvec3> Geom_CreateBall(const fvec3& ref_org, const float radius, const int32_t latitudeSegments, const int32_t longitudeSegments)
{
	std::list<fvec3> points;

	points.clear();
	std::vector<fvec3> verts;
	float phiStep = M_PI / latitudeSegments;
	float thetaStep = 2.0f * M_PI / longitudeSegments;

	for (int lat = 0; lat <= latitudeSegments; ++lat) {
		float phi = lat * phiStep;
		for (int lon = 0; lon <= longitudeSegments; ++lon) {
			float theta = lon * thetaStep;

			float x = ref_org.x + (radius * std::sin(phi) * std::cos(theta));
			float y = ref_org.y + (radius * std::cos(phi));
			float z = ref_org.z + (radius * std::sin(phi) * std::sin(theta));

			verts.push_back({ x, y, z });
		}
	}

	for (int lat = 0; lat < latitudeSegments; ++lat) {
		for (int lon = 0; lon < longitudeSegments; ++lon) {
			int v0 = lat * (longitudeSegments + 1) + lon;
			int v1 = v0 + 1;
			int v2 = (lat + 1) * (longitudeSegments + 1) + lon;
			int v3 = v2 + 1;

			points.push_back(verts[v0]);
			points.push_back(verts[v2]);
			points.push_back(verts[v1]);

			points.push_back(verts[v1]);
			points.push_back(verts[v2]);
			points.push_back(verts[v3]);

		}
	}
}
std::vector<fvec3> Geom_CreatePyramid(float size)
{
	std::vector<fvec3> pyramidPoints;

	pyramidPoints.emplace_back(0.0, 0.0, 0.0);
	pyramidPoints.emplace_back(size, 0.0, 0.0);
	pyramidPoints.emplace_back(size, 0.0, size);
	pyramidPoints.emplace_back(0.0, 0.0, size);

	pyramidPoints.emplace_back(size / 2.0, size * 1.5, size / 2.0);

	std::vector<std::vector<int>> triangles = {
		{0, 1, 2},  
		{0, 2, 3},  
		{0, 3, 4},  
		{0, 4, 1},  
		{1, 2, 4},  
		{2, 3, 4},  
		{3, 4, 1}  
	};

	std::vector<fvec3> sortedPoints;

	// Sort the points based on triangle indices
	for (const auto& triangle : triangles) {
		for (int index : triangle) {
			sortedPoints.push_back(pyramidPoints[index]);
		}
	}

	return sortedPoints;
}