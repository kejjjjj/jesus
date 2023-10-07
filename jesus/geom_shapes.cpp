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
std::vector<fvec3> Geom_CreatePyramid(const fvec3& ref_org, const fvec3& bounds, float rotation)
{
	std::vector<fvec3> pyramidVertices;

	constexpr int size = 4;

	float r = DEG2RAD(rotation);

	// Generate the base of the pyramid
	for (int i = 0; i < size; ++i) {
		float angle = 2 * 3.14159265359f * static_cast<float>(i) / static_cast<float>(size);
		fvec3 vertex;
		vertex.x = cos(angle+r) * bounds.x;
		vertex.y = sin(angle+r) * bounds.y;
		vertex.z = 0.0f;
		pyramidVertices.push_back(ref_org + vertex);
	}

	// Add the apex of the pyramid
	fvec3 apex;
	apex.x = 0.0f;
	apex.y = 0.0f; // Adjust the height of the pyramid as needed
	apex.z = bounds.z;
	
	apex += ref_org;
	
	pyramidVertices.push_back(ref_org + apex);

	// Create triangles from the base vertices and apex
	std::vector<fvec3> pyramidTriangles;

	for (int i = 1; i < size; ++i) {
		int nextIndex = (i + 1) % int(size);

		// Base triangles
		fvec3 triangle1[3] = { pyramidVertices[i], pyramidVertices[nextIndex], apex };
		pyramidTriangles.insert(pyramidTriangles.end(), std::begin(triangle1), std::end(triangle1));

		fvec3 triangle2[3] = { pyramidVertices[nextIndex], pyramidVertices[i], pyramidVertices[nextIndex] };
		pyramidTriangles.insert(pyramidTriangles.end(), std::begin(triangle2), std::end(triangle2));
	}

	return pyramidTriangles;
}